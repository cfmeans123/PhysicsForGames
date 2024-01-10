#pragma once

#include "PlatformCore.h"

#ifdef JM_ON_WINDOWS
#pragma warning (push)
#pragma warning (disable: 5054)
#include <eigen/Eigen/SparseCore>
#pragma warning (pop)
#endif

namespace jm::math
{
	template <typename TEdge, typename TVertex>
	class bidirectional_graph
	{
		using Index = Eigen::SparseMatrix<TEdge>::Index;

	protected :

		Eigen::SparseMatrix<TEdge> adjacency;
		std::vector<TVertex> vertices;

		template <typename, typename>
		friend class bidirectional_graph;

	public :

		explicit bidirectional_graph(std::vector<TVertex> vertices)
			: adjacency(vertices.size(), vertices.size())
			, vertices(vertices)
		{
		}

		template <typename TVertexIn, typename Fxn>
		bidirectional_graph(bidirectional_graph<TEdge, TVertexIn> const& otherGraph, Fxn&& function)
			: adjacency(otherGraph.adjacency)
			, vertices(otherGraph.vertices.size())
		{
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				vertices[i] = function(otherGraph.vertices[i]);
			}
		}

		void set_edge(size_t start, size_t end, TEdge value)
		{
			adjacency.insert(start, end) = value;
		}

		void add_vertex(TVertex value)
		{
			vertices.push_back(vertices);
			adjacency.conservativeResize(vertices.size(), vertices.size());
		}

		void add_vertices(std::vector<TVertex> values)
		{
			vertices.insert(vertices.end(), values.begin(), values.end());
			adjacency.conservativeResize(vertices.size(), vertices.size());
		}

		TEdge get_edge(size_t start, size_t end) const
		{
			return adjacency.coeff(start, end);
		}

		const TVertex& get_vertex(size_t index) const
		{
			return vertices[index];
		}

		void set_path(std::vector<size_t> path, TEdge pathValue, bool closed)
		{
			if (closed)
			{
				set_edge(path[path.size() - 1], path[0], pathValue);
			}

			for (size_t i = 0; i < path.size() - 1; ++i)
			{
				set_edge(path[i], path[i + 1], pathValue);
			}
		}

		void set_path(std::vector<size_t> path, std::vector<TEdge> pathValues, bool closed)
		{
			if (closed)
			{
				JM_MATH_ASSERT(pathValues.size() == path.size());
				set_edge(path[path.size() - 1], path[0], pathValues[path.size() - 1]);
			}
			else
			{
				JM_MATH_ASSERT(pathValues.size() == path.size() - 1);
			}

			for (size_t i = 0; i < path.size() - 1; ++i)
			{
				set_edge(path[i], path[i + 1], pathValues[i]);
			}
		}

		void clear_path(std::vector<size_t> path, bool closed)
		{
			auto keep_edge = [](const Index& row, const Index& col, const TEdge& value) -> bool
			{
				auto find_result = std::find(path.begin(), path.end(), row);
				if (find_result != path.end())
				{
					if (find_result == path.end() - 1)
					{
						return path.front() == col && closed;
					}

					if (*(find_result + 1) == col)
					{
						return false;
					}
				}
				return true;
			};

			adjacency.prune(keep_edge);
		}
	};
}
