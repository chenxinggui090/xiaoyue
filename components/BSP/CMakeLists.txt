set(src_dirs
            Chip_Info
            ADC_DMA
            TIMER)

set(include_dirs
            Chip_Info
            ADC_DMA
            TIMER)

set(requires
            Algorithm
            driver
            nvs_flash
            heap)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)