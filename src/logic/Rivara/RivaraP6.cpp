#include "RivaraP6.hpp"

namespace Rivara
{
    RivaraP6::RivaraP6(std::shared_ptr<IGraph> graph,
            vertex_descriptor hangingNode,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                hangingNode(hangingNode),
                TEdge(TEdge)
        {}

    void RivaraP6::Perform()
    {
        spdlog::debug("Rivara P6");
        auto vertices = graph -> GetAdjacentVertices(TEdge);

        auto hangingNodeEEdges = graph -> GetAdjacentVertices(hangingNode, NODELABEL_E);
        std::vector<vertex_descriptor> hangingNodeNeighbors;
        std::vector<vertex_descriptor> xd;
        for(auto eEdge : hangingNodeEEdges)
        {
            xd = graph->GetAdjacentVertices(eEdge);
            hangingNodeNeighbors.insert(hangingNodeNeighbors.end(), xd.begin(), xd.end());
        }

        std::vector<vertex_descriptor> hangingNodeNeighborsInTriangle;
        Intersection(hangingNodeNeighbors, vertices, hangingNodeNeighborsInTriangle);

        std::vector<vertex_descriptor> lastNodeSet;

        Rivara::RelativeComplementOfBInA(
            vertices, 
            hangingNodeNeighborsInTriangle,
            lastNodeSet);

        Pixel newEMiddleNode = GetNewEMiddleNode(graph, (*graph)[hangingNode], lastNodeSet[0]);
        Pixel newTNode = GetNewTNode();

        auto newTVertex = graph -> AddVertex(newTNode);
        auto newEMiddleVertex = graph -> AddVertex(newEMiddleNode);
        
        graph -> AddEdge(hangingNode, newEMiddleVertex);
        graph -> AddEdge(lastNodeSet[0], newEMiddleVertex);

        graph -> RemoveEdge(TEdge, hangingNodeNeighborsInTriangle[0]);
        graph -> AddEdge(TEdge, hangingNode);

        graph -> AddEdge(newTVertex, hangingNode);
        graph -> AddEdge(newTVertex, lastNodeSet[0]);
        graph -> AddEdge(newTVertex, hangingNodeNeighborsInTriangle[0]);

        (*graph)[TEdge].x = ((*graph)[hangingNodeNeighborsInTriangle[1]].x+(*graph)[hangingNode].x+(*graph)[lastNodeSet[0]].x)/3;
        (*graph)[TEdge].y = ((*graph)[hangingNodeNeighborsInTriangle[1]].y+(*graph)[hangingNode].y+(*graph)[lastNodeSet[0]].y)/3;
        (*graph)[newTVertex].x = ((*graph)[hangingNodeNeighborsInTriangle[0]].x+(*graph)[hangingNode].x+(*graph)[lastNodeSet[0]].x)/3;
        (*graph)[newTVertex].y = ((*graph)[hangingNodeNeighborsInTriangle[0]].y+(*graph)[hangingNode].y+(*graph)[lastNodeSet[0]].y)/3;

        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);

        (*graph)[TEdge].error = (*graph)[newTVertex].error = -1;
    }

    std::unique_ptr<std::vector<RivaraP6>> RivaraP6::FindAllMatches(std::shared_ptr<RivaraCachedGraph> g)
    {
       std::unique_ptr<std::vector<RivaraP6>> result = std::make_unique<std::vector<RivaraP6>>();
        //auto triangles = g -> GetCacheIterator(NODELABEL_T);
        auto triangles = TrianglesWhichMightHaveHangingNode(*g);
        for(auto triangle : triangles)
        {
            std::vector<vertex_descriptor> secondEEdges;
            std::vector<vertex_descriptor> commonEEdges;
            std::vector<vertex_descriptor> vertices;
            GetCommonEEdges(g, triangle, secondEEdges, commonEEdges, vertices);
            if(commonEEdges.size()==0)
            {
                std::vector<vertex_descriptor> hangingNodes;
                for(auto e : secondEEdges)
                {
                    auto tmp = g->GetAdjacentVertices(e);
                    hangingNodes.insert(hangingNodes.end(),tmp.begin(),tmp.end());
                }
                double hangingNodePoint1X = ((*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint1Y = ((*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                double hangingNodePoint2X = ((*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint2Y = ((*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                double hangingNodePoint3X = ((*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint3Y = ((*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                hangingNodes = where(hangingNodes, 
                [
                    &g,
                    &vertices,
                    hangingNodePoint1X,
                    hangingNodePoint1Y,
                    hangingNodePoint2X,
                    hangingNodePoint2Y,
                    hangingNodePoint3X,
                    hangingNodePoint3Y]
                    (vertex_descriptor v){
                        auto vNode = (*g)[v];
                        return v!=vertices[0]and v!=vertices[1] and v!=vertices[2]
                        and ((abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint1X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint1Y)<0.1)
                        or (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint2X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint2Y)<0.1)
                        or (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint3X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint3Y)<0.1))
                        ;});
                auto hangingNode0EEdges = g -> GetAdjacentVertices(hangingNodes[0], NODELABEL_E);
                auto hangingNode1EEdges = g -> GetAdjacentVertices(hangingNodes[1], NODELABEL_E);
                auto hangingNode2EEdges = g -> GetAdjacentVertices(hangingNodes[2], NODELABEL_E);
                
                std::vector<vertex_descriptor> hangingNode0EEdgesInThisTriangle;
                std::vector<vertex_descriptor> hangingNode1EEdgesInThisTriangle;
                std::vector<vertex_descriptor> hangingNode2EEdgesInThisTriangle;
                Intersection(hangingNode0EEdges, secondEEdges, hangingNode0EEdgesInThisTriangle);
                Intersection(hangingNode1EEdges, secondEEdges, hangingNode1EEdgesInThisTriangle);
                Intersection(hangingNode2EEdges, secondEEdges, hangingNode2EEdgesInThisTriangle);

                double L4 = (*g)[hangingNode0EEdgesInThisTriangle[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L5 = (*g)[hangingNode0EEdgesInThisTriangle[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L6 = (*g)[hangingNode1EEdgesInThisTriangle[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L7 = (*g)[hangingNode1EEdgesInThisTriangle[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L8 = (*g)[hangingNode2EEdgesInThisTriangle[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L9 = (*g)[hangingNode2EEdgesInThisTriangle[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                if((L4+L5)>=(L6+L7) and (L4+L5)>=(L8+L9))
                {
                    result->emplace_back(g,hangingNodes[0], triangle);
                }
                else if((L4+L5)>=(L6+L7) and (L4+L5)>=(L8+L9))
                {
                    result->emplace_back(g,hangingNodes[1], triangle);
                }
                else 
                {
                    result->emplace_back(g,hangingNodes[2], triangle);   
                }
            }
        }
        return result; 
    }
    

}