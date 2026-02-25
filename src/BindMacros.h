#pragma once


#define BIND_PROP(m_class, m_variant_type, m_name) \
    ClassDB::bind_method(D_METHOD("Get" #m_name), &m_class::Get##m_name); \
    ClassDB::bind_method(D_METHOD("Set" #m_name, "newVal"), &m_class::Set##m_name); \
    ClassDB::add_property(#m_class, PropertyInfo(m_variant_type, #m_name), "Set" #m_name, "Get" #m_name);

#define BIND_FUNC(m_class, m_name) \
    ClassDB::bind_method(D_METHOD(#m_name), &m_class::m_name);

#define BIND_SIG(m_class, m_type, m_name) \
	ADD_SIGNAL(MethodInfo(#m_name, PropertyInfo(Variant::m_type, #m_class)));

#define BIND_SIG_OVERIDE(m_class, m_type, m_name) \
	ADD_SIGNAL(MethodInfo(#m_name, PropertyInfo(Variant::m_type, #m_class)));

#define PREPEND_UNDERSCORE(m_name) "_" #m_name

//in .cpp _bind_methods()

#define BIND_VIRTUAL(m_class, m_name) \
    ClassDB::add_virtual_method(get_class_static(), MethodInfo(Variant::NIL, PREPEND_UNDERSCORE(m_name))); \
    ClassDB::bind_method(D_METHOD(#m_name), &m_class::m_name);

#define BIND_VIRTUAL_1(m_class, m_name, m_variant_type, arg_name) \
    ClassDB::add_virtual_method(get_class_static(), MethodInfo(Variant::NIL, PREPEND_UNDERSCORE(m_name), PropertyInfo(Variant::m_variant_type, #arg_name))); \
    ClassDB::bind_method(D_METHOD(#m_name, #arg_name), &m_class::m_name);

#define BIND_VIRTUAL_2(m_class, m_name, m_variant_type1, arg_name1, m_variant_type2, arg_name2) \
    ClassDB::add_virtual_method(get_class_static(), MethodInfo(Variant::NIL, PREPEND_UNDERSCORE(m_name), PropertyInfo(Variant::m_variant_type1, #arg_name1), PropertyInfo(Variant::m_variant_type2, #arg_name2))); \
    ClassDB::bind_method(D_METHOD(#m_name, #arg_name1, #arg_name2), &m_class::m_name);

// VOID versions - no return
#define BIND_BRIDGE_VOID(m_name)   \
    GDVIRTUAL0(_##m_name)           \
    void m_name();  \
    void m_name##BRIDGE() { if (GDVIRTUAL_CALL(_##m_name)) return; m_name(); }

#define BIND_BRIDGE_VOID_1(m_name, arg1_type, arg1_name) \
    GDVIRTUAL1(_##m_name, arg1_type)                      \
    void m_name(arg1_type arg1_name); \
    void m_name##BRIDGE(arg1_type arg1_name) { \
        if (GDVIRTUAL_CALL(_##m_name, arg1_name)) return; \
        m_name(arg1_name); \
    }

#define BIND_BRIDGE_VOID_2(m_name, arg1_type, arg1_name, arg2_type, arg2_name) \
    GDVIRTUAL2(_##m_name, arg1_type, arg2_type)           \
    void m_name(arg1_type arg1_name, arg2_type arg2_name); \
    void m_name##BRIDGE(arg1_type arg1_name, arg2_type arg2_name) { \
        if (GDVIRTUAL_CALL(_##m_name, arg1_name, arg2_name)) return; \
        m_name(arg1_name, arg2_name); \
    }

// in .h instead of regular config 
// i hate you HACK

#define BIND_BRIDGE(type, m_name)   \
    GDVIRTUAL0(_##m_name)           \
    type m_name();  \
    type m_name##BRIDGE() {if (type val = GDVIRTUAL_CALL(_OnGroundDash)) return val; return m_name(); }

#define BIND_BRIDGE_1(type, m_name, arg1_type, arg1_name) \
    GDVIRTUAL1(_##m_name, arg1_type)                      \
    type m_name(arg1_type arg1_name); \
    type m_name##BRIDGE(arg1_type arg1_name) { \
        if (type val = GDVIRTUAL_CALL(_##m_name, arg1_name)) return val; \
        return m_name(arg1_name); \
    }

#define BIND_BRIDGE_2(type, m_name, arg1_type, arg1_name, arg2_type, arg2_name) \
    GDVIRTUAL2(_##m_name, arg1_type, arg2_type)           \
    type m_name(arg1_type arg1_name, arg2_type arg2_name); \
    type m_name##BRIDGE(arg1_type arg1_name, arg2_type arg2_name) { \
        if (type val = GDVIRTUAL_CALL(_##m_name, arg1_name, arg2_name)) return val; \
        return m_name(arg1_name, arg2_name); \
    }

#define BIND_BRIDGE_3(type, m_name, arg1_type, arg1_name, arg2_type, arg2_name, arg3_type, arg3_name) \
    GDVIRTUAL3(_##m_name, arg1_type, arg2_type, arg3_type) \
    type m_name(arg1_type arg1_name, arg2_type arg2_name, arg3_type arg3_name); \
    type m_name##BRIDGE(arg1_type arg1_name, arg2_type arg2_name, arg3_type arg3_name) { \
        if (type val = GDVIRTUAL_CALL(_##m_name, arg1_name, arg2_name, arg3_name)) return val; \
        return m_name(arg1_name, arg2_name, arg3_name); \
    }