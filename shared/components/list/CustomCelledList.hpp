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
    concept ComponentCellRenderable = requires(T t, CellData cellData, UnityEngine::Transform* parent, RenderContext& ctx, RenderContextChildData& data) {
        T::render(parent, cellData, ctx, data);
    };

    template <typename DataSource, typename CustomTypeComponentCell, typename CellData, typename QCell>
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
                    QCell::render(cell->get_transform(), descriptor, tableContext, cellData);
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