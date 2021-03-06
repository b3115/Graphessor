#include "writers.hpp"
#include "CachedGraph.hpp"

DebugWriter::DebugWriter(std::string fileBaseName)
{
    this->fileBaseName = fileBaseName;
}
void DebugWriter::Write(std::string fileSuffix, IGraph &graph) 
{
    {
        myEdgeWriterWithNodeId<IGraph> w(graph);
        std::ofstream a(fileBaseName+fileSuffix);
        boost::write_graphviz(a, graph.GetGraph(), w);
    }
}