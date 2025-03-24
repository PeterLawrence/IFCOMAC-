/////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern std::string format_string(const AttributeValue& argument);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class = void>
struct is_ifc4_or_higher : std::false_type {};

template<class T>
struct is_ifc4_or_higher<T, std::void_t<decltype(T::IfcMaterialDefinition)>> : std::true_type { };

template <typename Schema, typename T>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool process_pset(const T* inst, const char* PSet, const char *Prop, std::string &aPropValue)
{
    AttributeValue  argument;

    std::string aPropName;
    std::string aPSetName;

    // Process an individual Property or Quantity set.
    if (auto pset = inst->template as<typename Schema::IfcPropertySet>())
    {
        if (!pset->Name()) {
            return false;
        }
        aPSetName = pset->Name().get();
        //std::cout << "PSET " << aPSetName << std::endl;

        if (aPSetName.compare(PSet) == 0)
        {
            auto ps = pset->HasProperties();
            for (auto it = ps->begin(); it != ps->end(); ++it) {
                auto& p = *it;
                if (auto singleval = p->template as<typename Schema::IfcPropertySingleValue>())
                {
                    if constexpr (is_ifc4_or_higher<Schema>::value) {
                        if (!singleval->Name()) {
                            continue;
                        }
                        aPropName = *singleval->Name();
                    }
                    if constexpr (!is_ifc4_or_higher<Schema>::value) {
                        aPropName = singleval->Name();
                    }
                    if (aPropName.compare(Prop) == 0)
                    {
                        if (!singleval->NominalValue()) {
                            aPropValue = "-";
                        }
                        else
                        {
                            auto the_data = singleval->NominalValue()->template as<IfcUtil::IfcBaseClass>()->data();
                            argument = the_data.get_attribute_value(0);
                            auto argument2 = singleval->Unit();
                            aPropValue = format_string(argument);
                        }
                        //std::cout << "PSet " << aPSetName << " " << aPropName << " " << aPropValue << std::endl;
                        return (true);
                    }
                }
            }
        }
    }
    return (false);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Schema, typename T>
void process_pset(const T* inst)
{
    AttributeValue  argument;
    std::string aPropValue;
    std::string aPropName;
    std::string aPSetName;

    // Process an individual Property or Quantity set.
    if (auto pset = inst->template as<typename Schema::IfcPropertySet>()) {
        if (!pset->Name()) {
            return;
        }
        aPSetName = pset->Name().get();
        auto ps = pset->HasProperties();
        for (auto it = ps->begin(); it != ps->end(); ++it) {
            auto& p = *it;
            if (auto singleval = p->template as<typename Schema::IfcPropertySingleValue>()) {
                ;
                if constexpr (is_ifc4_or_higher<Schema>::value) {
                    if (!singleval->Name()) {
                        continue;
                    }
                    aPropName = *singleval->Name();
                }
                if constexpr (!is_ifc4_or_higher<Schema>::value) {
                    aPropName = singleval->Name();
                }
                if (!singleval->NominalValue()) {
                    aPropValue = "-";
                }
                else
                {
                    argument = singleval->NominalValue()->template as<IfcUtil::IfcBaseClass>()->data().get_attribute_value(0);
                    aPropValue = format_string(argument);
                }
                std::cout << "PSet " << aPSetName << " " << aPropName << " " << aPropValue << std::endl;
            }
        }
    }
    if (auto qset = inst->template as<typename Schema::IfcElementQuantity>()) {
        if (!qset->Name()) {
            return;
        }
        aPropName = qset->Name().get();
        auto qs = qset->Quantities();
        for (auto it = qs->begin(); it != qs->end(); ++it) {
            auto& q = *it;
            if (q->template as<typename Schema::IfcPhysicalSimpleQuantity>() && q->data().get_attribute_value(3).type() == IfcUtil::Argument_DOUBLE) {
                argument = q->data().get_attribute_value(3);
                //argument = AttributeValue(v);
                const double v = argument;
                aPropValue = std::to_string(v);
            }
            std::cout << "QSet " << aPSetName << " " << aPropName << " " << aPropValue << std::endl;
        }
    }
    if constexpr (is_ifc4_or_higher<Schema>::value) {
        if (auto extprops = inst->template as<typename Schema::IfcExtendedProperties>()) {
            // @todo
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Schema>
void get_psets_s(const typename Schema::IfcObjectDefinition* inst)
{
    // Extracts the property definitions for an IFC instance. 
    if (auto tyob = inst->template as<typename Schema::IfcTypeObject>()) {
        if (tyob->HasPropertySets())
        {
            auto defs = *tyob->HasPropertySets();
            for (auto it = defs->begin(); it != defs->end(); ++it) {
                auto& def = *it;
                process_pset<Schema>(def);
            }
        }
    }
    if constexpr (is_ifc4_or_higher<Schema>::value) {
        if (auto mdef = inst->template as<typename Schema::IfcMaterialDefinition>()) {
            auto defs = mdef->HasProperties();
            for (auto it = defs->begin(); it != defs->end(); ++it) {
                auto& def = *it;
                process_pset<Schema>(def);
            }
        }
        if (auto pdef = inst->template as<typename Schema::IfcProfileDef>()) {
            auto defs = pdef->HasProperties();
            for (auto it = defs->begin(); it != defs->end(); ++it) {
                auto& def = *it;
                process_pset<Schema>(def);
            }
        }
    }
    if (auto ob = inst->template as<typename Schema::IfcObject>()) {
        if constexpr (is_ifc4_or_higher<Schema>::value) {
            auto rels = ob->IsTypedBy();
            for (auto it = rels->begin(); it != rels->end(); ++it) {
                auto& rel = *it;
                get_psets_s<Schema>(rel->RelatingType());
            }
        }
        {
            auto rels = ob->IsDefinedBy();
            for (auto it = rels->begin(); it != rels->end(); ++it) {
                auto& rel = *it;
                if (auto bytype = rel->template as<typename Schema::IfcRelDefinesByType>()) {
                    get_psets_s<Schema>(bytype->RelatingType());
                }
                else if (auto byprops = rel->template as<typename Schema::IfcRelDefinesByProperties>()) {
                    process_pset<Schema>(byprops->RelatingPropertyDefinition());
                }
            }
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Schema>
bool get_psets_s(const typename Schema::IfcObjectDefinition* inst, const char* PSet, const char *Prop, std::string &aPropValue)
{
    if (auto ob = inst->template as<typename Schema::IfcObject>())
    {
        auto rels = ob->IsDefinedBy();
        for (auto it = rels->begin(); it != rels->end(); ++it) {
            auto& rel = *it;
            if (auto byprops = rel->template as<typename Schema::IfcRelDefinesByProperties>()) {
                if (process_pset<Schema>(byprops->RelatingPropertyDefinition(), PSet, Prop, aPropValue))
                {
                    return true;
                }
            }
        }
        auto typedby = ob->IsTypedBy();
        for (auto it = typedby->begin(); it != typedby->end(); ++it)
        {
            auto& rel = *it;
            auto reltype = rel->RelatingType();
            auto relprope = reltype->HasPropertySets();
            if (relprope.is_initialized())
            {
                for (auto it2 = relprope.get()->begin(); it2 != relprope.get()->end(); ++it2) {
                    auto& rel2 = *it2;
                    if (auto pset = rel2->as<IFCSchema::IfcPropertySet>())
                    {
                        if (process_pset<Schema>(pset, PSet, Prop, aPropValue))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return (false);
}