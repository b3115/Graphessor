#ifndef CACHEDGRAPH_HPP
#define CACHEDGRAPH_HPP

#include "mygraph.hpp"

class CachedGraph : public IGraph
{
    private:
        std::map<std::string, std::unique_ptr<std::set<vertex_descriptor>>> typeToVerticesCache;
        std::unique_ptr<PixelGraph> graph;
        void EnsureCacheExists(std::string type);
        void InsertCacheElement(std::string type, vertex_descriptor v);
        void ChangeCachedElementType(vertex_descriptor v, std::string type1, std::string type2);
    public:
        CachedGraph();
        void AddEdge(vertex_descriptor v1,vertex_descriptor v2) override;
        vertex_descriptor AddVertex(Pixel p) override;
        void RemoveEdge(vertex_descriptor v1,vertex_descriptor v2) override;
        void ChangeVertexType(vertex_descriptor v, std::string type) override;
        Pixel& operator[](vertex_descriptor v) override;
        const std::set<vertex_descriptor>& GetCacheIterator(std::string type);
        std::vector<vertex_descriptor> GetAdjacentVertices(vertex_descriptor v);
        PixelGraph GetGraph();
};

#endif /* CACHEDGRAPH_HPP */