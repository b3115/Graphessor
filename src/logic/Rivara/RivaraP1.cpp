#include "RivaraP1.hpp"
#include <algorithm>
#include <cassert>

namespace Rivara
{
    RivaraP1::RivaraP1(std::shared_ptr<RivaraCachedGraph> graph,
            vertex_descriptor EEdge,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image) 
            :
                graph(graph), 
                EEdge(EEdge),
                TEdge(TEdge),
                image(image)
        {}

    void RivaraP1::Perform()
    {
        spdlog::debug("Rivara P1");
        graph->DeregisterMarkedElement(TEdge);
        auto nodes = graph -> GetAdjacentVertices(EEdge);
        auto lastNodes = graph -> GetAdjacentVertices(TEdge);
        spdlog::debug("{} {} {}", lastNodes[0],  lastNodes[1], lastNodes[2]);
        assert(lastNodes.size()==3 && "P1 TEdge has not 3 nodes");
        auto lastNode = where(lastNodes,[&nodes](vertex_descriptor v){return nodes[0]!=v and nodes[1]!=v;}).front();

        auto newTNode = GetNewTNode();
        auto newNNode = GetNewNNode(graph, nodes, image);
        auto newMiddleENode = GetNewEMiddleNode(graph, newNNode, lastNode);
        auto newENode = GetNewENode(graph, EEdge);

        vertex_descriptor newNNodeVertex = graph -> AddVertex(newNNode);
        vertex_descriptor newENodeVertex = graph -> AddVertex(newENode);
        vertex_descriptor newTNodeVertex = graph -> AddVertex(newTNode);
        vertex_descriptor newMiddleENodeVertex = graph -> AddVertex(newMiddleENode);

        graph -> RemoveEdge(EEdge, nodes[0]);
        graph -> AddEdge(EEdge, newNNodeVertex);

        graph -> AddEdge(nodes[0], newENodeVertex);
        graph -> AddEdge(newNNodeVertex, newENodeVertex);

        graph -> RemoveEdge(TEdge, nodes[0]);
        graph -> AddEdge(TEdge, newNNodeVertex);

        graph -> AddEdge(nodes[0], newTNodeVertex);
        graph -> AddEdge(newNNodeVertex, newTNodeVertex);
        graph -> AddEdge(lastNode, newTNodeVertex);

        graph -> AddEdge(newMiddleENodeVertex, lastNode);
        graph -> AddEdge(newMiddleENodeVertex, newNNodeVertex);
        (*graph)[EEdge].x = ((*graph)[nodes[1]].x + newNNode.x) / 2;
        (*graph)[EEdge].y = ((*graph)[nodes[1]].y + newNNode.y) / 2;
        (*graph)[EEdge].attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL((*graph)[nodes[1]], newNNode));
        (*graph)[newENodeVertex].x = ((*graph)[nodes[0]].x+newNNode.x)/2;
        (*graph)[newENodeVertex].y = ((*graph)[nodes[0]].y+newNNode.y)/2;
        (*graph)[TEdge].x = ((*graph)[nodes[1]].x+newNNode.x+(*graph)[lastNode].x)/3;
        (*graph)[TEdge].y = ((*graph)[nodes[1]].y+newNNode.y+(*graph)[lastNode].y)/3;
        (*graph)[newTNodeVertex].x = ((*graph)[nodes[0]].x+newNNode.x+(*graph)[lastNode].x)/3;
        (*graph)[newTNodeVertex].y = ((*graph)[nodes[0]].y+newNNode.y+(*graph)[lastNode].y)/3;

        (*graph)[TEdge].attributes -> SetBool(RIVARA_ATTRIBUTE_R, false);
        (*graph)[TEdge].error = (*graph)[newTNodeVertex].error = -1;

        spdlog::debug("{} {} {} {}", nodes[0],  nodes[1], lastNode, newNNodeVertex);
        graph->RegisterNotComputedElement(TEdge);
        graph->RegisterNotComputedElement(newTNodeVertex);
    }

    std::unique_ptr<std::vector<RivaraP1>> RivaraP1::FindAllMatches(std::shared_ptr<RivaraCachedGraph> g, std::shared_ptr<Image> image) 
    {
        auto result = std::make_unique<std::vector<RivaraP1>>();
        //auto triangles = g -> GetCacheIterator(NODELABEL_T);
        auto triangles = g -> GetMarkedElements();
        for(auto t : triangles)
        {
            if(!(*g)[t].attributes -> GetBool(RIVARA_ATTRIBUTE_R)) //if not marked
                continue;
            std::vector<vertex_descriptor> secondEEdges;
            std::vector<vertex_descriptor> commonEEdges;
            std::vector<vertex_descriptor> vertices;
            GetCommonEEdges(g, t, secondEEdges, commonEEdges, vertices);
            
            if(commonEEdges.size() == 3) //checks whether there is any hanging node (3 means none)
            {
                auto maybeBestEdge = RivaraP1::GetBestEdge(g, commonEEdges);
                if(maybeBestEdge.has_value())
                {
                    result->emplace_back(g, maybeBestEdge.value(), t, image);
                    return result;
                }
            }
        }
        return result;
    }

    std::optional<vertex_descriptor> RivaraP1::GetBestEdge(std::shared_ptr<CachedGraph> g, const std::vector<vertex_descriptor>& EEdges)
    {
        std::vector<Pixel> commonEEdgesPixels; 
        for(auto EEdge : EEdges)
        {
            commonEEdgesPixels.push_back((*g)[EEdge]);
        }
        std::sort(commonEEdgesPixels.begin(), commonEEdgesPixels.end(), EdgeComparator);
        double maxLength = commonEEdgesPixels.back().attributes->GetDouble(RIVARA_ATTRIBUTE_L);
        for(auto e : EEdges)
        {
            Pixel p = (*g)[e];
            if(p.attributes->GetBool(RIVARA_ATTRIBUTE_B) 
                and p.attributes->GetDouble(RIVARA_ATTRIBUTE_L)==maxLength)
            {    
                return e; //longest edge at the border
            }
        }
        for(auto e : EEdges)
        {
            Pixel p = (*g)[e];
            if(p.attributes->GetDouble(RIVARA_ATTRIBUTE_L)==maxLength)
            {
                auto commonNodes = g -> GetAdjacentVertices(e, NODELABEL_N);
                if(true
                    and !(*g)[commonNodes[0]].attributes->GetBool(RIVARA_ATTRIBUTE_HN)
                    and !(*g)[commonNodes[1]].attributes->GetBool(RIVARA_ATTRIBUTE_HN))
                {    
                    return e; //longest edge without hanging nodes
                }
            }
        }
        return {}; //suitable edge not found
    }

    std::vector<uint8_t> RivaraP1::Serialize()
    {
        throw NotImplementedException();
    }
}