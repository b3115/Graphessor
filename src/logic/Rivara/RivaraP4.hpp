#ifndef Rivara_P4_HPP
#define Rivara_P4_HPP

#include "Productions.hpp"
#include "RivaraUtils.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP4 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor NNode;
            vertex_descriptor TEdge;
        public:
            RivaraP4(std::shared_ptr<IGraph> graph,
                vertex_descriptor NNode,
                vertex_descriptor TEdge);
            void Perform();
            static std::unique_ptr<std::vector<RivaraP4>> FindAllMatches(std::shared_ptr<RivaraCachedGraph> g);
            std::vector<uint8_t> Serialize();   
    }; 
}

#endif /* Rivara_P4_HPP */
