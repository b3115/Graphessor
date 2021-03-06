#include "RivaraP3.hpp"
#include<cassert>

namespace Rivara
{
    RivaraP3::RivaraP3(std::shared_ptr<RivaraCachedGraph> graph,
            vertex_descriptor EEdgeToBreak,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image) 
            :
                graph(graph), 
                EEdgeToBreak(EEdgeToBreak),
                TEdge(TEdge),
                image(image)
        {}

    void RivaraP3::Perform()
    {
        spdlog::debug("Rivara P3");
        auto nodes = graph -> GetAdjacentVertices(EEdgeToBreak);
        assert(nodes.size()==2 && "nodes should have 2 elements");
        auto vertices = graph -> GetAdjacentVertices(TEdge);
        spdlog::debug("{}", TEdge);
        spdlog::debug("{} {} {}", vertices[0],  vertices[1], vertices[2]);
        std::vector<vertex_descriptor> lastNodeSet;
        Rivara::RelativeComplementOfBInA(
            vertices, nodes, lastNodeSet);

        auto lastNode = lastNodeSet[0];
        assert(lastNodeSet.size()==1 && "lastNodeSet should have 1 element");
        auto newTNode = GetNewTNode();
        auto newNNode = GetNewNNode(graph, nodes, image);
        auto newMiddleENode = GetNewEMiddleNode(graph, newNNode, lastNode);
        auto newENode = GetNewENode(graph, EEdgeToBreak);

        vertex_descriptor newNNodeVertex = graph -> AddVertex(newNNode);
        vertex_descriptor newENodeVertex = graph -> AddVertex(newENode);
        vertex_descriptor newTNodeVertex = graph -> AddVertex(newTNode);
        vertex_descriptor newMiddleENodeVertex = graph -> AddVertex(newMiddleENode);

        graph -> RemoveEdge(EEdgeToBreak, nodes[0]);
        graph -> AddEdge(EEdgeToBreak, newNNodeVertex);

        graph -> AddEdge(nodes[0], newENodeVertex);
        graph -> AddEdge(newNNodeVertex, newENodeVertex);

        graph -> RemoveEdge(TEdge, nodes[0]);
        graph -> AddEdge(TEdge, newNNodeVertex);

        graph -> AddEdge(nodes[0], newTNodeVertex);
        graph -> AddEdge(newNNodeVertex, newTNodeVertex);
        graph -> AddEdge(lastNode, newTNodeVertex);
        
        graph -> AddEdge(newMiddleENodeVertex, lastNode);
        graph -> AddEdge(newMiddleENodeVertex, newNNodeVertex);

        spdlog::debug("{} {} {} {}", nodes[0],  nodes[1], lastNode, newNNodeVertex);

        (*graph)[EEdgeToBreak].x = ((*graph)[nodes[1]].x+newNNode.x)/2;
        (*graph)[EEdgeToBreak].y = ((*graph)[nodes[1]].y+newNNode.y)/2;
        (*graph)[newENodeVertex].x = ((*graph)[nodes[0]].x+newNNode.x)/2;
        (*graph)[newENodeVertex].y = ((*graph)[nodes[0]].y+newNNode.y)/2;
        (*graph)[TEdge].x = ((*graph)[nodes[1]].x+newNNode.x+(*graph)[lastNode].x)/3;
        (*graph)[TEdge].y = ((*graph)[nodes[1]].y+newNNode.y+(*graph)[lastNode].y)/3;
        (*graph)[newTNodeVertex].x = ((*graph)[nodes[0]].x+newNNode.x+(*graph)[lastNode].x)/3;
        (*graph)[newTNodeVertex].y = ((*graph)[nodes[0]].y+newNNode.y+(*graph)[lastNode].y)/3;

        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);
        (*graph)[EEdgeToBreak].attributes->SetDouble(RIVARA_ATTRIBUTE_L, NL((*graph)[nodes[1]],newNNode));
        (*graph)[TEdge].error = (*graph)[newTNodeVertex].error = -1;
        graph->RegisterNotComputedElement(TEdge);
        graph->RegisterNotComputedElement(newTNodeVertex);
        
    }

    std::unique_ptr<std::vector<RivaraP3>> RivaraP3::FindAllMatches(std::shared_ptr<RivaraCachedGraph> g, std::shared_ptr<Image> image)
    {
       std::unique_ptr<std::vector<RivaraP3>> result = std::make_unique<std::vector<RivaraP3>>();
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
                std::set<vertex_descriptor> tempENodes1; 
                auto hangingNodeNNeighboursEEdgeNeighbours = g -> GetAdjacentVertices(hangingNodeNeighbors[0], NODELABEL_E);
                tempENodes1.insert(hangingNodeNNeighboursEEdgeNeighbours.begin(), hangingNodeNNeighboursEEdgeNeighbours.end());
                hangingNodeNNeighboursEEdgeNeighbours = g -> GetAdjacentVertices(hangingNodeNeighbors[1], NODELABEL_E);
                tempENodes1.insert(hangingNodeNNeighboursEEdgeNeighbours.begin(), hangingNodeNNeighboursEEdgeNeighbours.end());
                std::vector<vertex_descriptor> possibleHangingNodes;
                for(auto EEdge : tempENodes1)
                {
                    auto tmp = g -> GetAdjacentVertices(EEdge);
                    possibleHangingNodes.insert(possibleHangingNodes.end(), tmp.begin(), tmp.end());
                }
                double expectedX =
                    ((*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;  
                double expectedY =
                    ((*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                auto hangingNode = first(possibleHangingNodes, [&g, expectedX, expectedY](vertex_descriptor v){
                    return true
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_X) - expectedX) < 0.1
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) - expectedY) < 0.1;
                    });
                
                auto hangingNodeENeighbors = g -> GetAdjacentVertices(hangingNode, NODELABEL_E);
                std::vector<vertex_descriptor> hangingNodeENeighborsInThisTriangle;
                Rivara::Intersection(
                    hangingNodeENeighbors, tempENodes1, hangingNodeENeighborsInThisTriangle);
                double lengthOfSplitEdge = (*g)[hangingNodeENeighborsInThisTriangle[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_L)
                 + (*g)[hangingNodeENeighborsInThisTriangle[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_L);
                if(true
                    and lengthOfSplitEdge<(*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and (*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)>=(*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and 
                    (
                        (*g)[commonEEdges[0]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                        or 
                        (
                            EdgeHasNoHangingNodes(*g, commonEEdges[0])
                            and 
                            !(
                                (*g)[commonEEdges[1]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                                and 
                                (*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)==(*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                            )
                        )
                    )
                )
                {
                    result -> emplace_back(g, commonEEdges[0], triangle, image);
                }
                else if(true
                    and lengthOfSplitEdge<(*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and (*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)>=(*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and 
                    (
                        (*g)[commonEEdges[1]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                        or 
                        (
                            EdgeHasNoHangingNodes(*g, commonEEdges[1])
                            and 
                            !(
                                (*g)[commonEEdges[0]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                                and 
                                (*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)==(*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                            )
                        )
                    )
                )
                {
                    result -> emplace_back(g, commonEEdges[1], triangle, image);
                }
            }
        }
        return result; 
    }




}