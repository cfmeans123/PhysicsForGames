#include "VisualGeometry.h"

#include "Math/Graph.h"

namespace jm
{
	namespace Visual
	{
		template <size_t Dimension, typename Scalar>
		RawBuffer GenerateLine(const InputLayout& layout, math::vectorN<Dimension, Scalar> const& start, math::vectorN<Dimension, Scalar> const& end, math::colour3_f32 const& colour)
		{
			ComponentLayout vertexData(layout);

			{
				std::vector<math::vectorN<Dimension, Scalar>> componentData
				{
					start, end
				};
				vertexData.AddComponent(0, componentData);
			}

			{
				std::vector<math::colour3_f32> componentData
				{
					colour, colour
				};
				vertexData.AddComponent(1, componentData);
			}

			return vertexData.GetVertexBuffer();
		}

		using index_sequence = std::vector<size_t>;

		index_sequence GetSequence(size_t a, size_t b, size_t stepSize = 1)
		{
			if (a == b)
			{
				return { a };
			}

			if (a + stepSize == b)
			{
				return { a, b };
			}

			JM_VISUAL_ASSERT(std::gcd(b - a, stepSize) == stepSize);

			index_sequence numbers;
			for (size_t i = a; i <= b; i += stepSize)
			{
				numbers.push_back(i);
			}
			return numbers;
		}

		index_sequence GetSequence(size_t a, index_sequence middle, size_t b)
		{
			index_sequence sequence = { a, b };
			sequence.insert(sequence.begin() + 1, middle.begin(), middle.end());
			return sequence;
		}

		template <size_t Dimension, typename Scalar>
		class GeometryGraph : math::bidirectional_graph<size_t, math::vectorN<Dimension, Scalar>>
		{
			static_assert((Dimension == 2) != (Dimension == 3));

			using Vertex = math::vectorN<Dimension, Scalar>;
			using Super = math::bidirectional_graph<size_t, Vertex>;
			using Tri = math::vector3<size_t>;
			using Quad = math::vector4<size_t>;

			std::vector<Tri> Triangles;

		public:

			explicit GeometryGraph(std::vector<Vertex>&& vertices)
				: Super(vertices)
			{}
			
			template <size_t DimensionIn, typename ScalarIn, typename Fxn>
			GeometryGraph(GeometryGraph<DimensionIn, ScalarIn> const& geometry, Fxn&& function)
				: Super(geometry, function)
				, Triangles(geometry.GetTriangles())
			{
			}

			auto GetTriangleCount() const
			{
				return Triangles.size();
			}

			std::vector<Vertex> const& GetVertices() const
			{
				return Super::vertices;
			}

			const auto& GetTriangles() const
			{
				return Triangles;
			}

			const auto& GetVertex(size_t index) const
			{
				return Super::get_vertex(index);
			}

			index_sequence AddVertexSet(std::vector<Vertex> const& vertexSet)
			{
				index_sequence indices = GetSequence(Super::vertices.size(), Super::vertices.size() + vertexSet.size() - 1);
				Super::add_vertices(vertexSet);
				return indices;
			}

			void AddTriangle(Tri face)
			{
				auto faceIndex = Triangles.size();
				Triangles.push_back(face);
				Super::set_path({ face.x, face.y, face.z }, faceIndex, true);
			}

			auto AddQuadrangle(Quad face)
			{
				AddTriangle({ face.x, face.y, face.z });
				AddTriangle({ face.x, face.z, face.w });
			}

			void AddFan(size_t centre, index_sequence const& outer, bool closed)
			{
				JM_VISUAL_ASSERT(outer.size() > 1);

				for (size_t i = 0; i < outer.size() - 1; ++i)
				{
					AddTriangle({ centre, outer[i], outer[i + 1] });
				}

				if (closed)
				{
					AddTriangle({ centre, outer[outer.size() - 1], outer[0] });
				}
			};

			void AddStrip(index_sequence const& up, index_sequence const& down, bool closed = false)
			{
				size_t size = std::min(up.size(), down.size());

				for (size_t i = 0; i < size - 1; ++i)
				{
					AddQuadrangle({ up[i], down[i], down[i + 1], up[i + 1] });
				}

				if (up.size() > size)
				{
					JM_VISUAL_ASSERT(!closed, "Doesn't make sense with uneven strip");
					
					AddFan(down[size - 1], index_sequence{ up.data() + size - 1, up.data() + up.size() }, false);
				}
				else if (down.size() > size)
				{
					JM_VISUAL_ASSERT(!closed, "Doesn't make sense with uneven strip");

					AddFan(up[size - 1], index_sequence{ down.data() + size - 1, down.data() + down.size() }, false);
				}
				else if (closed)
				{
					AddQuadrangle({ up[size - 1], down[size - 1], down[0], up[0] });
				}
			};
		};

		template <size_t Dimension>
		using GeometryGraph_f32 = GeometryGraph<Dimension, f32>;

		template <size_t Dimension>
		RawBuffer GeneratePerFaceMeshes(const GeometryGraph_f32<Dimension>& topology, const InputLayout& layout, bool perFaceNormal = false)
		{
			perFaceNormal;
			static_assert(Dimension == 2 || Dimension == 3, "Unsuppored dimensionality!");

			ComponentLayout vertexData(layout);
			size_t vertexCount = 3 * topology.GetTriangleCount();

			std::vector<math::vectorN<Dimension, f32>> positions(vertexCount);

			size_t faceIndex = 0;
			for (auto& face : topology.GetTriangles())
			{
				positions[3 * faceIndex + 0] = topology.GetVertex(face[0]);
				positions[3 * faceIndex + 1] = topology.GetVertex(face[1]);
				positions[3 * faceIndex + 2] = topology.GetVertex(face[2]);
				++faceIndex;
			}

			std::vector<math::colour3_f32> colours(vertexCount);
			for (size_t i = 0; i < vertexCount; ++i)
			{
				const f32 yValue = positions[i].y;
				const f32 t = (yValue + 1.f) * 0.5f;
				colours[i] = math::lerp(t, math::red, math::yellow, math::green, math::cyan, math::blue);
			}

			vertexData.AddComponent(0, positions);
			vertexData.AddComponent(1, colours);

			return vertexData.GetVertexBuffer();

		}

		template <size_t Dimension>
		RawBuffer GenerateCoordinateAxes(const InputLayout& layout)
		{
			static_assert(Dimension == 2 || Dimension == 3, "Unsuppored dimensionality!");

			auto addLine = [layout](RawBuffer& vertexList, math::vectorN<Dimension, f32> const& start
				, math::vectorN<Dimension, f32> const& end, math::colour3_f32 const& colour)
			{
				vertexList.Append(GenerateLine(layout, start, end, colour));
			};

			constexpr f32 farAway = (f32)((int)(1 << 10));
			RawBuffer coordinateAxes{ };
			if constexpr (Dimension == 2)
			{
				addLine(coordinateAxes, { -farAway, 0.0f }, { farAway, 0.0f }, math::red);
				addLine(coordinateAxes, { 0.0f, -farAway }, { 0.0f, farAway }, math::green);
			}
			else
			{
				addLine(coordinateAxes, { -farAway, 0.0f, 0.0f }, { farAway, 0.0f, 0.0f }, math::red);
				addLine(coordinateAxes, { 0.0f, -farAway, 0.0f }, { 0.0f, farAway, 0.0f }, math::green);
				addLine(coordinateAxes, { 0.0f, 0.0f, -farAway }, { 0.0f, 0.0f, farAway }, math::blue);
			}

			constexpr f32 max = 10.0f;
			constexpr f32 inc = max / 10.0f;
			f32 generatedf32 = 0.0f;
			std::array<f32, 10> fRng;
			std::ranges::generate(fRng, [&generatedf32]() mutable { return generatedf32 += inc; });
			if constexpr (Dimension == 2)
			{
				for (f32 x : fRng)
				{
					addLine(coordinateAxes, { -x, -max }, { -x, max }, math::gray);
					addLine(coordinateAxes, { x, -max }, { x, max }, math::gray);
				}
				for (f32 y : fRng)
				{
					addLine(coordinateAxes, { -max, -y }, { max, -y }, math::gray);
					addLine(coordinateAxes, { -max, y }, { max, y }, math::gray);
				}
			}
			else
			{
				for (f32 x : fRng)
				{
					addLine(coordinateAxes, { -x, 0.0f, -max }, { -x, 0.0f, max }, math::gray);
					addLine(coordinateAxes, { x, 0.0f, -max }, { x, 0.0f, max }, math::gray);
				}
				for (f32 z : fRng)
				{
					addLine(coordinateAxes, { -max, 0.0f, -z }, { max, 0.0f, -z }, math::gray);
					addLine(coordinateAxes, { -max, 0.0f, z }, { max, 0.0f, z }, math::gray);
				}
			}

			return coordinateAxes;
		}

		math::vector2_f32 RootOfUnity(size_t k, size_t n)
		{
			const f32 theta = math::two_pi<f32>() * ((f32)(k) / (f32)(n));
			return { std::cos(theta), std::sin(theta) };
		}

		GeometryGraph_f32<2> SimplePolygonGeometry(f32 diameter, size_t sides)
		{
			JM_VISUAL_ASSERT(sides >= 3);

			const f32 radius = 0.5f * diameter;

			std::vector<math::vector2_f32> vertices;
			vertices.push_back(math::vector2_f32{ 0.f });
			for (u32 i = 0; i < sides; ++i)
			{
				vertices.push_back(radius * RootOfUnity(i, sides));
			}

			GeometryGraph_f32<2> geometry(std::move(vertices));
			for (size_t i = 1; i < sides; ++i)
			{
				geometry.AddTriangle({ 0, i, i + 1 });
			}
			geometry.AddTriangle({ 0, sides, 1 });
			return geometry;
		}

		template <size_t Dimension, typename Scalar>
		std::vector<math::vectorN<Dimension, Scalar>> GetLinearInterpolants(math::vectorN<Dimension, Scalar> const& a
			, math::vectorN<Dimension, Scalar> const& b, size_t sections, bool return_ends = false)
		{
			if (sections == 0)
			{
				if (return_ends)
				{
					return { a,b };
				}
				else
				{
					return {};
				}
			}

			std::vector<math::vectorN<Dimension, Scalar>> vertices;

			if (return_ends)
			{
				vertices.push_back(a);
			}

			math::vectorN<Dimension, Scalar> v = b - a;
			math::vectorN<Dimension, Scalar> dir = normalize(v);
			Scalar deltaDisp = length(v) / (sections + 1);
			for (u32 i = 1; i <= sections; ++i)
			{
				vertices.push_back(a + dir * (i * deltaDisp));
			}

			if (return_ends)
			{
				vertices.push_back(b);
			}
			return vertices;
		}

		template <size_t Dimension, typename Scalar>
		std::vector<math::vectorN<Dimension, Scalar>> GetArcedInterpolants(math::vectorN<Dimension, Scalar> const& a
			, math::vectorN<Dimension, Scalar> const& b, size_t sections, bool return_ends = false)
		{
			if (sections == 0)
			{
				if (return_ends)
				{
					return { a,b };
				}
				else
				{
					return {};
				}
			}

			Scalar angle = math::angle(a, b);
			if (std::abs(angle) > std::numeric_limits<Scalar>::epsilon())
			{
				std::vector<math::vectorN<Dimension, Scalar>> vertices;

				if (return_ends)
				{
					vertices.push_back(a);
				}

				Scalar deltaAlpha = math::one<Scalar>() / (sections + 1);
				for (u32 i = 1; i <= sections; ++i)
				{
					const Scalar t = i * deltaAlpha;
					const Scalar s = math::one<Scalar>() - t;
					const Scalar aScale = std::sin(s * angle) / std::sin(angle);
					const Scalar bScale = std::sin(t * angle) / std::sin(angle);

					vertices.push_back(aScale * a + bScale * b);
				}

				if (return_ends)
				{
					vertices.push_back(b);
				}
				return vertices;
			}
			else
			{
				return GetLinearInterpolants(a, b, sections, return_ends);
			}
		}


		using edge = std::pair<size_t, size_t>;
		using SubDivisionEdgeMap = std::map<edge, index_sequence>;

		template <size_t Dimension>
		GeometryGraph_f32<Dimension> Subdivide(GeometryGraph_f32<Dimension> const& geometry, size_t subdivisions, bool arced)
		{
			JM_VISUAL_ASSERT(subdivisions >= 2);

			SubDivisionEdgeMap edgeMap;

			auto vertices = geometry.GetVertices();
			GeometryGraph_f32<Dimension> newGeometry(std::move(vertices));

			auto addInterpolatedEdge = [&newGeometry, &edgeMap, arced](edge edge, size_t subdivisions)
			{
				auto first = newGeometry.GetVertex(edge.first);
				auto second = newGeometry.GetVertex(edge.second);
				if (arced)
				{
					edgeMap[edge] = newGeometry.AddVertexSet(GetArcedInterpolants(first, second, subdivisions - 1));
				}
				else
				{
					edgeMap[edge] = newGeometry.AddVertexSet(GetLinearInterpolants(first, second, subdivisions - 1));
				}
				auto edgeVertices = edgeMap[edge];
				std::reverse(edgeVertices.begin(), edgeVertices.end());
				edgeMap[{ edge.second, edge.first }] = edgeVertices;
			};

			for (auto face : geometry.GetTriangles())
			{
				edge firstEdge = { face[0], face[1] };
				if (!edgeMap.contains(firstEdge))
				{
					addInterpolatedEdge(firstEdge, subdivisions);
				}
				edge secondEdge = { face[1], face[2] };
				if (!edgeMap.contains(secondEdge))
				{
					addInterpolatedEdge(secondEdge, subdivisions);
				}
				edge thirdEdge = { face[2], face[0] };
				if (!edgeMap.contains(thirdEdge))
				{
					addInterpolatedEdge(thirdEdge, subdivisions);
				}
			}

			for (auto face : geometry.GetTriangles())
			{
				auto a = geometry.GetVertex(face[0]);
				auto b = geometry.GetVertex(face[1]);
				auto c = geometry.GetVertex(face[2]);
				auto aAngle = math::angle(b - a, c - a);
				auto bAngle = math::angle(c - b, a - b);
				auto cAngle = math::pi<f32>() - aAngle - bAngle;
				size_t centre = 0;
				size_t left = 0;
				size_t right = 0;
				if (aAngle >= bAngle)
				{
					if (cAngle >= aAngle)
					{
						//c largest
						centre = face[2];
						left = face[0];
						right = face[1];
					}
					else
					{
						//a largest
						centre = face[0];
						left = face[1];
						right = face[2];
					}
				}
				else
				{
					if (cAngle >= bAngle)
					{
						//c largest
						centre = face[2];
						left = face[0];
						right = face[1];
					}
					else
					{
						//b largest
						centre = face[1];
						left = face[2];
						right = face[0];
					}
				}

				auto const& leftVertices = edgeMap[{centre, left}];
				auto const& rightVertices = edgeMap[{centre, right}];

				//peak triangle
				newGeometry.AddTriangle({ centre, leftVertices[0], rightVertices[0] });

				//middle triangles
				index_sequence lastVertices = { leftVertices[0], rightVertices[0] };
				for (size_t i = 1; i < subdivisions - 1; ++i)
				{
					auto first = newGeometry.GetVertex(leftVertices[i]);
					auto second = newGeometry.GetVertex(rightVertices[i]);
					index_sequence interSequence;
					if (arced)
					{
						interSequence = newGeometry.AddVertexSet(GetArcedInterpolants(first, second, i));
					}
					else
					{
						interSequence = newGeometry.AddVertexSet(GetLinearInterpolants(first, second, i));
					}
					index_sequence currentVertices = GetSequence(leftVertices[i], interSequence, rightVertices[i]);

					newGeometry.AddStrip(lastVertices, currentVertices, false);

					lastVertices = currentVertices;
				}

				//base triangles
				edge baseEdge = { left, right };
				index_sequence currentVertices = GetSequence(baseEdge.first, edgeMap[baseEdge], baseEdge.second);

				newGeometry.AddStrip(lastVertices, currentVertices, false);
			}

			return newGeometry;
		}

		RawBuffer GenerateCoordinateAxes2(const InputLayout& layout)
		{
			return GenerateCoordinateAxes<2>(layout);
		}

		RawBuffer GenerateCoordinateAxes3(const InputLayout& layout)
		{
			return GenerateCoordinateAxes<3>(layout);
		}

		RawBuffer GenerateBox(const InputLayout& layout, f32 diameter)
		{
			const float extent = diameter * 0.5f;

			std::vector<math::vector2<f32>> vertices{
				{-extent, -extent},
				{extent, -extent},
				{extent, extent},
				{-extent, extent}
			};

			GeometryGraph_f32<2> boxGraph(std::move(vertices));

			boxGraph.AddQuadrangle({ 0, 1, 2, 3 });

			return GeneratePerFaceMeshes(Subdivide(boxGraph, 5, false), layout);
		}

		RawBuffer GenerateCube(const InputLayout& layout, f32 diameter)
		{
			const float extent = diameter * 0.5f;

			std::vector<math::vector3<f32>> vertices{
				{-extent, -extent, -extent},
				{extent, -extent, -extent},
				{extent, extent, -extent},
				{-extent, extent, -extent},
				{-extent, extent, extent},
				{extent, extent, extent},
				{extent, -extent, extent},
				{-extent, -extent, extent}
			};

			GeometryGraph_f32<3> cubeGraph(std::move(vertices));

			cubeGraph.AddQuadrangle({ 3, 2, 1, 0 }); //bottom
			cubeGraph.AddQuadrangle({ 7, 6, 5, 4 }); //top
			cubeGraph.AddQuadrangle({ 1, 2, 5, 6 }); //front
			cubeGraph.AddQuadrangle({ 2, 3, 4, 5 }); //right
			cubeGraph.AddQuadrangle({ 0, 7, 4, 3 }); //back
			cubeGraph.AddQuadrangle({ 0, 1, 6, 7 }); //left


			return GeneratePerFaceMeshes(Subdivide(cubeGraph, 5, false), layout);
		}


		GeometryGraph_f32<2> PolygonGeometry(f32 diameter, size_t sides, size_t rings, bool arced)
		{
			JM_VISUAL_ASSERT(rings != 0);

			auto geometery = SimplePolygonGeometry(diameter, sides);
			if (rings == 1)
			{
				return geometery;
			}

			return Subdivide(geometery, rings, arced);
		}

		RawBuffer GeneratePolygon(const InputLayout& layout, f32 diameter, size_t sides, size_t rings)
		{
			auto polygon = PolygonGeometry(diameter, sides, rings, false);
			return GeneratePerFaceMeshes(polygon, layout);
		}

		RawBuffer GenerateDisk(const InputLayout& layout, f32 diameter, size_t slices, size_t rings)
		{
			auto arcedPolygon = PolygonGeometry(diameter, slices, rings, true);
			return GeneratePerFaceMeshes(arcedPolygon, layout);
		}

		RawBuffer GenerateDisk(const InputLayout& layout, f32 diameter)
		{
			return GenerateDisk(layout, diameter, 5, 5);
		}

		RawBuffer GeneratePlane(const InputLayout& layout)
		{
			auto addLine = [layout](RawBuffer& vertexList, math::vector3_f32 const& start
				, math::vector3_f32 const& end, math::colour3_f32 const& colour)
			{
				vertexList.Append(GenerateLine(layout, start, end, colour));
			};

			constexpr f32 farAway = (f32)((int)(1 << 10));
			RawBuffer coordinateAxes{ };
			addLine(coordinateAxes, { -farAway, 0.0f, 0.0f }, { farAway, 0.0f, 0.0f }, math::white);
			addLine(coordinateAxes, { 0.0f, 0.0f, -farAway }, { 0.0f, 0.0f, farAway }, math::white);
			addLine(coordinateAxes, { -1.f, 0.0f, -1.f }, { 1.f, 0.0f, -1.f }, math::magenta);
			addLine(coordinateAxes, { 1.f, 0.0f, -1.f }, { 1.f, 0.0f, 1.f }, math::magenta);
			addLine(coordinateAxes, { 1.f, 0.0f, 1.f }, { -1.f, 0.0f, 1.f }, math::magenta);
			addLine(coordinateAxes, { -1.f, 0.0f, 1.f }, { -1.f, 0.0f, -1.f }, math::magenta);

			return coordinateAxes;
		}

		RawBuffer GenerateSphere(const InputLayout& layout, f32 diameter, size_t slices, size_t layers)
		{
			JM_VISUAL_ASSERT(slices > 2);
			JM_VISUAL_ASSERT(layers > 1);

			const f32 radius = 0.5f * diameter;

			const f32 deltaPhi = math::pi<f32>() / layers;
			const f32 deltaTheta = math::two_pi<f32>() / slices;

			GeometryGraph_f32<3> sphere({ {0.f, radius, 0.f} });

			index_sequence lastIndices;
			{
				std::vector<math::vector3_f32> cap(slices);
				for (size_t i = 0; i < slices; ++i)
				{
					const f32 theta = deltaTheta * i;
					cap[i] = math::cartesian_from_spherical(radius, theta, deltaPhi);
				}

				lastIndices = sphere.AddVertexSet(cap);
				sphere.AddFan(0, lastIndices, true);
			}

			for (size_t i = 2; i < layers; ++i)
			{
				std::vector<math::vector3_f32> layer(slices);
				const f32 phi = deltaPhi * i;
				for (size_t j = 0; j < slices; ++j)
				{
					const f32 theta = deltaTheta * j;
					layer[j] = math::cartesian_from_spherical(radius, theta, phi);
				}
			
				index_sequence layerIndices = sphere.AddVertexSet(layer);
				sphere.AddStrip(lastIndices, layerIndices, true);
				lastIndices = layerIndices;
			}

			{
				std::reverse(lastIndices.begin(), lastIndices.end());
				index_sequence centre = sphere.AddVertexSet({ {0.f, -radius, 0.f} });
				sphere.AddFan(centre.front(), lastIndices, true);
			}

			return GeneratePerFaceMeshes(sphere, layout);
		}

		RawBuffer GenerateSphere(const InputLayout& layout, f32 diameter)
		{
			return GenerateSphere(layout, diameter, 20, 18);
		}
	}
}