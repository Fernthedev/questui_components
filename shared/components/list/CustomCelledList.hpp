#pragma once

#include "shared/RootContainer.hpp"
#include "CustomTypeTable.hpp"

#include "UnityEngine/Vector2.hpp"

#include "questui/shared/CustomTypes/Components/List/CustomCellListWrapper.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include <utility>
#include <vector>
#include <concepts>

namespace QUC {

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

    template <typename DataSource,typename QCell,
            typename CustomTypeComponentCell = typename DataSource::CustomQUCCustomCellT, // Boiler plate defaults
            typename CellData = typename DataSource::CustomQUCDescriptorT>
    requires(
            QUC::CustomTypeList::IsValidQUCTableData<DataSource> &&
            // Component data
            QUC::CustomTypeList::IsValidQUCTableCell<CustomTypeComponentCell> &&
            ComponentCellRenderable<QCell, CellData>)
    struct RecycledTable {
        const Key key;
        const std::vector<CellData> cellDatas;
        const CustomTypeList::QUCTableInitData initData;

        constexpr RecycledTable(std::vector<CellData> const &cellData, CustomTypeList::QUCTableInitData const &initData) : cellDatas(cellData), initData(initData)  {}
        constexpr RecycledTable(std::initializer_list<CellData> const &cellData, CustomTypeList::QUCTableInitData const &initData) : cellDatas(cellData), initData(initData) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& dataSource = data.getData<DataSource*>();
            if (!dataSource) {
                typename DataSource::CreateCellCallback buildCell =
                        [&data, &dataSource](typename DataSource::CustomQUCCustomCellT* cell, bool created, typename DataSource::CustomQUCDescriptorT const& descriptor){
                            auto& tableContext = data.template getChildContext([&dataSource]{
                                return dataSource->get_transform();
                            });

                    auto& cellData = tableContext.getChildData(cell->key);
                    if constexpr(requires(QCell) {QCell::render;}) {
                        QCell::render(cell->get_transform(), descriptor, tableContext, cellData);
                    } else {
                        // construct a QCell and give it more QUC-like data
                        auto cellTransform = cell->get_transform();

                        QCell& qCell = cellData.template getData<QCell>();
                        auto& cellContext = cellData.template getChildContext([cellTransform]{return cellTransform;});

                        qCell.render(descriptor, cellContext);
                    }
                };

                dataSource = QUC::CustomTypeList::CreateCustomList<DataSource>(&ctx.parentTransform, buildCell, initData);
                dataSource->descriptors = cellDatas;
                dataSource->Init(initData);
            }

            auto& childContext = data.template getChildContext([dataSource]{
                return dataSource->get_transform();
            });


            return &childContext.parentTransform;
        }

    };

}