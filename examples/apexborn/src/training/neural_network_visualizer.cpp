#include "neural_network_visualizer.h"

#include "math/math_transform.h"

#include <imgui.h>
#include <queue>
#include <ranges>
#include <unordered_map>

namespace ab
{

    namespace
    {

        constexpr ImU32 CanvasBackground = IM_COL32(30, 35, 45, 255);
        constexpr ImU32 CanvasBorder = IM_COL32(85, 100, 120, 255);
        constexpr ImU32 GridColor = IM_COL32(110, 125, 145, 30);
        constexpr ImU32 PolylineColor = IM_COL32(100, 200, 245, 255);
        constexpr ImU32 ControlPointColor = IM_COL32(100, 200, 245, 255);
        constexpr ImU32 HoveredPointColor = IM_COL32(255, 205, 90, 255);
        constexpr ImU32 DraggedPointColor = IM_COL32(255, 125, 100, 255);
        constexpr ImU32 SnapPreviewColor = IM_COL32(90, 230, 155, 255);
        constexpr ImU32 ControlPointOutline = IM_COL32(15, 20, 30, 255);

        using NodePositions = std::unordered_map<NodeId, ImVec2>;
        void layoutColumn(const std::vector<NodeGene>& nodes, size_t columnIndex, size_t columnCount, const ImVec2& origin, const ImVec2& canvasSize, NodePositions& positions)
        {
            constexpr float padding = 40.0f;

            const float availableWidth = std::max(0.0f, canvasSize.x - padding * 2.0f);
            const float availableHeight = std::max(0.0f, canvasSize.y - padding * 2.0f);
            const float normalizedX = columnCount > 1 ? static_cast<float>(columnIndex) / static_cast<float>(columnCount - 1) : 0.5f;
            const float x = origin.x + padding + normalizedX * availableWidth;
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                const float normalizedY = static_cast<float>(i + 1) / static_cast<float>(nodes.size() + 1);
                const float y = origin.y + padding + normalizedY * availableHeight;
                positions[nodes[i].id] = ImVec2(x, y);
            }
        }

        std::vector<std::vector<NodeGene>> calculateColumns(const Genome& genome)
        {
            const auto nodes = genome.getNodes();
            const auto connections = genome.getConnections();

            std::unordered_map<uint32_t, size_t> depth;
            std::unordered_map<uint32_t, size_t> inDegree;
            std::unordered_map<uint32_t, std::vector<uint32_t>> outgoing;

            for (const NodeGene& node : nodes)
            {
                depth[node.id] = 0;
                inDegree[node.id] = 0;
            }

            for (const ConnectionGene& connection : connections)
            {
                if (!connection.enabled)
                {
                    continue;
                }
                outgoing[connection.fromNode].push_back(connection.toNode);
                ++inDegree[connection.toNode];
            }

            std::queue<uint32_t> queue;
            for (const NodeGene& node : nodes)
            {
                if (inDegree[node.id] == 0)
                {
                    queue.push(node.id);
                }
            }

            while (!queue.empty())
            {
                const uint32_t current = queue.front();
                queue.pop();
                for (const uint32_t destination : outgoing[current])
                {
                    depth[destination] = std::max(depth[destination], depth[current] + 1);
                    if (--inDegree[destination] == 0)
                    {
                        queue.push(destination);
                    }
                }
            }

            // Reserve a dedicated final column for output neurons.
            size_t maxHiddenDepth = 0;
            for (const NodeGene& node : nodes)
            {
                if (node.type == NodeType::Hidden)
                {
                    maxHiddenDepth = std::max(maxHiddenDepth, depth[node.id]);
                }
            }

            // Inputs + hidden layers + outputs.
            const size_t outputColumn = maxHiddenDepth + 1;
            std::vector<std::vector<NodeGene>> columns(outputColumn + 1);
            for (const NodeGene& node : nodes)
            {
                size_t column = 0;
                switch (node.type)
                {
                    case NodeType::Input:
                    case NodeType::Bias:
                        column = 0;
                        break;

                    case NodeType::Hidden:
                        column = std::max<size_t>(1, depth[node.id]);
                        break;

                    case NodeType::Output:
                        column = outputColumn;
                        break;
                }
                columns[column].push_back(node);
            }
            return columns;
        }

        void renderConnections(ImDrawList* drawList, const Genome& genome, const NodePositions& positions)
        {
            constexpr ImU32 PositiveColor = IM_COL32(65, 200, 130, 180);
            constexpr ImU32 NegativeColor = IM_COL32(235, 90, 100, 180);
            constexpr ImU32 DisabledColor = IM_COL32(110, 115, 125, 70);

            for (const ConnectionGene& connection : genome.getConnections())
            {
                const auto from = positions.find(connection.fromNode);
                const auto to = positions.find(connection.toNode);

                if (from == positions.end() || to == positions.end())
                {
                    continue;
                }

                ImU32 color = DisabledColor;
                float thickness = 1.0f;

                if (connection.enabled)
                {
                    color = connection.weight >= 0.0 ? PositiveColor : NegativeColor;
                    thickness = aiko::math::clamp(static_cast<float>(std::abs(connection.weight)), 1.0f, 4.0f);
                }
                drawList->AddLine(from->second, to->second, color, thickness);
            }
        }

        ImU32 getNodeColor(NodeType type, const double* activation)
        {
            if (activation != nullptr)
            {
                const float intensity = std::clamp(static_cast<float>(std::abs(*activation)), 0.0f, 1.0f);
                const float base = 0.18f;
                const float strength = base + intensity * 0.82f;

                if (*activation >= 0.0)
                {
                    return IM_COL32(35, static_cast<int>(230.0f * strength), static_cast<int>(210.0f * strength), 255);
                }
                return IM_COL32(static_cast<int>(245.0f * strength), 40, 65, 255);
            }

            switch (type)
            {
                case NodeType::Input:   return IM_COL32(70, 150, 230, 255);
                case NodeType::Bias:    return IM_COL32(230, 180, 70, 255);
                case NodeType::Hidden:  return IM_COL32(170, 110, 220, 255);
                case NodeType::Output:  return IM_COL32(70, 200, 140, 255);
            }
            return IM_COL32(130, 130, 130, 255);
        }

    }

    void NeuralNetworkVisualizer::render(bool& open, const Genome& genome, std::span<const NodeActivation> activations, const char* modeLabel)
    {

        if (!ImGui::Begin("NeuralNetworkVisualizer", &open))
        {
            ImGui::End();
            return;
        }

        ImGui::TextUnformatted(modeLabel);

        const std::vector<NodeGene> nodes = genome.getNodes();

        std::unordered_map<NodeId, double> activationLookup;
        activationLookup.reserve(activations.size());
        for (const NodeActivation& activation : activations)
        {
            activationLookup[activation.id] = activation.value;
        }

        auto inputGenomesView = nodes | std::ranges::views::filter([](const NodeGene& gene)-> bool { return gene.type == NodeType::Input || gene.type == NodeType::Bias; });
        auto hiddenGenomesView = nodes | std::ranges::views::filter([](const NodeGene& gene)-> bool { return gene.type == NodeType::Hidden; });
        auto outputGenomesView = nodes | std::ranges::views::filter([](const NodeGene& gene)-> bool { return gene.type == NodeType::Output; });

        const std::vector<NodeGene> inputGenomes(inputGenomesView.begin(), inputGenomesView.end());
        const std::vector<NodeGene> hiddenGenomes(hiddenGenomesView.begin(), hiddenGenomesView.end());
        const std::vector<NodeGene> outputGenomes(outputGenomesView.begin(), outputGenomesView.end());

        static ImVec2 scrolling(0.0f, 0.0f);

        ImGui::Text("Mouse Right: drag to scroll.");
        ImGui::Text("Inputs: %zu  |  Hidden: %zu  |  Outputs: %zu", inputGenomes.size(), hiddenGenomes.size(), outputGenomes.size());

        ImGui::Separator();

        // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
        ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
        if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
        if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
        ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

        // Draw border and background color
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

        // This will catch our interactions
        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        const bool is_hovered = ImGui::IsItemHovered(); // Hovered
        const bool is_active = ImGui::IsItemActive();   // Held
        const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
        const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

        const float mouse_threshold_for_pan = 0.0f;
        if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
        {
            scrolling.x += io.MouseDelta.x;
            scrolling.y += io.MouseDelta.y;
        }

        draw_list->PushClipRect(canvas_p0, canvas_p1, true);

        // Draw NeuralNetwork
        {
            const float padding = 40.0f;
            const float topY = origin.y + padding;
            const float availableHeight = std::max(0.0f, canvas_sz.y - padding * 2.0f);

            std::vector<std::vector<NodeGene>> columns = calculateColumns(genome);
            NodePositions positions;
            for (size_t column = 0; column < columns.size(); ++column)
            {
                layoutColumn(columns[column], column, columns.size(), origin, canvas_sz, positions);
            }

            renderConnections(draw_list, genome, positions);
            for (const NodeGene& node : nodes)
            {
                const auto it = positions.find(node.id);
                if (it == positions.end())
                {
                    continue;
                }

                const auto activationIt = activationLookup.find(node.id);

                const double* activation = activationIt != activationLookup.end() ? &activationIt->second : nullptr;

                const ImU32 color = getNodeColor(node.type, activation);

                constexpr float NodeRadius = 8.0f;

                draw_list->AddCircleFilled(it->second, NodeRadius, color);
                draw_list->AddCircle(it->second, NodeRadius, IM_COL32(20, 25, 35, 255), 0, 1.5f);

                const ImVec2 mouse = ImGui::GetIO().MousePos;

                const float dx = mouse.x - it->second.x;
                const float dy = mouse.y - it->second.y;

                if (ImGui::IsItemHovered() && dx * dx + dy * dy <= NodeRadius * NodeRadius)
                {
                    ImGui::BeginTooltip();

                    ImGui::Text("Node ID: %llu", static_cast<unsigned long long>(node.id));
                    ImGui::Text("Type: %s", node.type == NodeType::Input ? "Input" : node.type == NodeType::Hidden ? "Hidden" : node.type == NodeType::Output ? "Output" : "Bias");

                    if (activation != nullptr)
                    {
                        ImGui::Text("Activation: %.5f", *activation);
                    }
                    else
                    {
                        ImGui::TextDisabled("No live activation");
                    }

                    ImGui::EndTooltip();
                }
            }
        }

        draw_list->PopClipRect();

        ImGui::End();

    }

}