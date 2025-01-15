set(src_dirs
            hcalculateNormalization
            WindowEnergy)

set(include_dirs
            hcalculateNormalization
            WindowEnergy)

set(requires
            )

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)