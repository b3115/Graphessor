#include "RivaraP2.hpp"

namespace Rivara
{
    RivaraP2::RivaraP2(std::shared_ptr<RivaraCachedGraph> graph,
            vertex_descriptor hangingNode,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                hangingNode(hangingNode),
                TEdge(TEdge)
        {}

    void RivaraP2::Perform()
    {
        spdlog::debug("Rivara P2");
        auto EEdges = graph -> GetAdjacentVertices(hangingNode);
        std::vector<vertex_descriptor> HNNeighbors;
        for(auto e : EEdges)
        {
            auto nodes = where(graph->GetAdjacentVertices(e), [this](vertex_descriptor v){return v!=hangingNode;});
            HNNeighbors.insert(HNNeighbors.end(), nodes.begin(), nodes.end());
        }
        auto vertices = graph->GetAdjacentVertices(TEdge);
        spdlog::debug("{} {} {}", vertices[0],  vertices[1], vertices[2]);
        vertex_descriptor lastNode = where(vertices, [&HNNeighbors](vertex_descriptor v){return v!=HNNeighbors[0] and v!=HNNeighbors[1];})[0];

        Pixel newENode = GetNewEMiddleNode(graph, (*graph)[hangingNode], lastNode);
        Pixel newTNode = GetNewTNode();
        vertex_descriptor newENodeVertex = graph -> AddVertex(newENode);
        vertex_descriptor newTNodeVertex = graph -> AddVertex(newTNode);

        (*graph)[hangingNode].attributes -> SetBool(RIVARA_ATTRIBUTE_HN, false);
        (*graph)[TEdge].attributes -> SetBool(RIVARA_ATTRIBUTE_R, false);

        //oldTEdge neighbors
        graph -> RemoveEdge(HNNeighbors[0], TEdge);
        graph -> AddEdge(hangingNode, TEdge);

        //newEEdge neighbors
        graph -> AddEdge(hangingNode, newENodeVertex);
        graph -> AddEdge(lastNode, newENodeVertex);

        //newTEdge neighbors
        graph -> AddEdge(newTNodeVertex, HNNeighbors[0]);
        graph -> AddEdge(newTNodeVertex, hangingNode);
        graph -> AddEdge(newTNodeVertex, lastNode);
        spdlog::debug("{} {} {} {}", HNNeighbors[0],  HNNeighbors[1], lastNode, hangingNode);
        (*graph)[TEdge].x = ((*graph)[HNNeighbors[1]].x+(*graph)[hangingNode].x+(*graph)[lastNode].x)/3;
        (*graph)[TEdge].y = ((*graph)[HNNeighbors[1]].y+(*graph)[hangingNode].y+(*graph)[lastNode].y)/3;
        (*graph)[newTNodeVertex].x = ((*graph)[HNNeighbors[0]].x+(*graph)[hangingNode].x+(*graph)[lastNode].x)/3;
        (*graph)[newTNodeVertex].y = ((*graph)[HNNeighbors[0]].y+(*graph)[hangingNode].y+(*graph)[lastNode].y)/3;

        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);
        (*graph)[TEdge].error = (*graph)[newTNodeVertex].error = -1;
        graph->RegisterNotComputedElement(TEdge);
        graph->RegisterNotComputedElement(newTNodeVertex);
    }


    std::unique_ptr<std::vector<RivaraP2>> RivaraP2::FindAllMatches(std::shared_ptr<RivaraCachedGraph> g)
    {
        std::unique_ptr<std::vector<RivaraP2>> result = std::make_unique<std::vector<RivaraP2>>();
        //auto triangles = g -> GetCacheIterator(NODELABEL_T);
        auto triangles = TrianglesWhichMightHaveHangingNode(*g);
        for(auto triangle : triangles)
        {
            std::vector<vertex_descriptor> secondEEdges;
            std::vector<vertex_descriptor> commonEEdges;
            std::vector<vertex_descriptor> vertices;
            GetCommonEEdges(g, triangle, secondEEdges, commonEEdges, vertices);
            if(commonEEdges.size()==2)
            {
                auto tempNNodes1 = g -> GetAdjacentVertices(commonEEdges[0]);
                auto tempNNodes2 = g -> GetAdjacentVertices(commonEEdges[1]);
                std::vector<vertex_descriptor> intersection;
                Rivara::Intersection(
                    tempNNodes1, tempNNodes2, intersection);
                std::vector<vertex_descriptor> hangingNodeNeighbors;
                hangingNodeNeighbors = where(vertices, [&intersection](vertex_descriptor v){return v!=intersection[0];});
                auto tempENodes1 = g -> GetAdjacentVertices(hangingNodeNeighbors[0], NODELABEL_E);
                auto tempENodes2 = g -> GetAdjacentVertices(hangingNodeNeighbors[1], NODELABEL_E);
                tempENodes1.insert(tempENodes1.end(), tempENodes2.begin(), tempENodes2.end());
                std::set<vertex_descriptor> possibleHangingNodes;
                for(auto EEdge : tempENodes1)
                {
                    auto tmp = g -> GetAdjacentVertices(EEdge, NODELABEL_N);
                    possibleHangingNodes.insert(tmp.begin(), tmp.end());
                }
                double expectedX =
                    ((*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;  
                double expectedY =
                    ((*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                auto hangingNode = (*(where(possibleHangingNodes, [&g, expectedX, expectedY](vertex_descriptor v){
                    return true
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_X) - expectedX) < 0.1
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) - expectedY) < 0.1;
                    }).begin()));
                auto hangingNodeENeighbors = g -> GetAdjacentVertices(hangingNode, NODELABEL_E);
                std::vector<vertex_descriptor> hangingNodeENeighborsInThisTriangle;
                Rivara::Intersection(
                    hangingNodeENeighbors, tempENodes1, hangingNodeENeighborsInThisTriangle);
                double lengthOfSplitEdge = (*g)[hangingNodeENeighborsInThisTriangle[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_L)
                 + (*g)[hangingNodeENeighborsInThisTriangle[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_L);
                if(true
                and lengthOfSplitEdge>=(*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                and lengthOfSplitEdge>=(*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                )
                result -> emplace_back(g, hangingNode, triangle);
            }
        }
        return result;
    }
}