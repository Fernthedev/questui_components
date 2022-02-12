#pragma once

#include "shared/RootContainer.hpp"
#include "CustomTypeTable.hpp"

#include "UnityEngine/Vector2.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include <utility>
#include <vector>
#include <concepts>

namespace QUC {

    template<typename HMUICell, typename CellData, typename CellComp>
    concept HMUICellQUC = requires(HMUICell cell, CellData const& cellData, CellComp& cellComp, RenderContext& ctx) {
        cell.render(cellData, ctx, cellComp);
    };

    template<typename T, typename CellData>
    concept ComponentCellRenderable = requires(T t) {
        std::is_trivially_default_constructible_v<T>;
    } &&
    // Render methods
    (
    requires(T t, CellData cellData, UnityEngine::Transform* cellTransform, RenderContext& tableCtx, RenderContextChildData& cellCtxData) {
        T::render(cellTransform, cellData, tableCtx, cellCtxData);
    } || requires(T t, CellData cellData, RenderContext& cellCtx) {
        t.render(cellData, cellCtx);
    });

    template <typename DataSource, typename CellData = typename DataSource::CustomQUCDescriptorT>
    struct RecycledTableRenderState {
        DataSource* dataSource;
        // TODO: Shared ptr to avoid so many copies?
        std::vector<CellData> cellDatas;
        bool cellDatasInited = false;
    };

    template <typename DataSource,typename QCell,
            typename CustomTypeComponentCell = typename DataSource::CustomQUCCustomCellT, // Boiler plate defaults
            typename CellData = typename DataSource::CustomQUCDescriptorT,
            typename ListType = std::vector<CellData>>
    requires(
            QUC::CustomTypeList::IsValidQUCTableData<DataSource> &&
            // Component data
            QUC::CustomTypeList::IsValidQUCTableCell<CustomTypeComponentCell> &&
            ComponentCellRenderable<QCell, CellData>)
    struct RecycledTable {
        using RenderState = RecycledTableRenderState<DataSource, CellData>;

        const Key key;
        ListType initCellDatas;
        const CustomTypeList::QUCTableInitData initData;

        constexpr RecycledTable(std::span<CellData> const cellData, CustomTypeList::QUCTableInitData const &initData) : initCellDatas(cellData), initData(initData)  {}
        constexpr RecycledTable(std::initializer_list<CellData> cellData, CustomTypeList::QUCTableInitData const &initData) : initCellDatas(cellData), initData(initData) {}

        constexpr UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) const {
            auto& tableState = data.getData<RenderState>();
            auto& dataSource = tableState.dataSource;
            auto& cellDatas = tableState.cellDatas;

            if (!dataSource) {
                if (!tableState.cellDatasInited) {
                    cellDatas = initCellDatas;
                    tableState.cellDatasInited = true;
                }

                typename DataSource::CreateCellCallback buildCell =
                        [&data, &dataSource](typename DataSource::CustomQUCCustomCellT* cell, bool created, typename DataSource::CustomQUCDescriptorT const& descriptor){
                            auto& tableContext = data.template getChildContext([&dataSource]{
                                return dataSource->get_transform();
                            });

                    // Each cell in the table has a key (at least in the QUC sense)
                    // We use the key to uniquely identify it and get the data rendered on that cell
                    // We then call the QUC Component (or the QuestUI static method) and update the cell's data
                    // with the descriptor
                    auto& cellData = tableContext.getChildData(cell->key);
                    if constexpr(requires(QCell) {QCell::render;}) {
                        QCell::render(cell->get_transform(), descriptor, tableContext, cellData);
                    } else {
                        // construct a QCell and give it more QUC-like data
                        auto cellTransform = cell->get_transform();

                        // Get the QUC component cell stored in RenderCtx
                        // This works because a QUC Cell stores the components such as Button and
                        // their keys are unique and stored in the cellContext
                        QCell& qCell = cellData.template getData<QCell>();
                        auto& cellContext = cellData.template getChildContext([cellTransform]{return cellTransform;});

                        qCell.render(descriptor, cellContext);

                        if constexpr(HMUICellQUC<typename DataSource::CustomQUCCustomCellT, typename DataSource::CustomQUCDescriptorT, QCell>) {
                            cell->render(descriptor, cellContext, qCell);
                        }
                    }
                };

                dataSource = QUC::CustomTypeList::CreateCustomList<DataSource>(&ctx.parentTransform, buildCell, initData);
                dataSource->descriptors = reinterpret_cast<std::vector<CellData const>*>(&cellDatas);
                dataSource->Init(initData);
            } else {
                dataSource->descriptors = reinterpret_cast<std::vector<CellData const>*>(&cellDatas);
                dataSource->tableView->ReloadData();
            }

            auto& childContext = data.template getChildContext([dataSource]{
                return dataSource->get_transform();
            });


            return &childContext.parentTransform;
        }

        std::vector<CellData>& getStatefulVector(RenderContext& ctx) const {
            auto& data = ctx.getChildDataOrCreate(key);
            auto& tableState = data.getData<RenderState>();
            auto& cellDatas = tableState.cellDatas;

            tableState.cellDatasInited = true;

            return cellDatas;
        }

    };

}