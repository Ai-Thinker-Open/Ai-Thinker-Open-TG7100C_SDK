src =Split(''' 
    hal/sensor_hal.c
    hal/sensor_drv_api.c
    drv/drv_humi_bosch_bme280.c
    drv/drv_acc_bosch_bma253.c
    drv/drv_baro_bosch_bmp280.c
    drv/drv_acc_gyro_st_lsm6dsl.c
    drv/drv_baro_st_lps22hb.c
    drv/drv_acc_mir3_da217.c
    drv/drv_als_ps_liteon_ltr553.c
    drv/drv_temp_humi_sensirion_shtc1.c
''')
component =aos_component('sensor', src)


global_includes =Split(''' 
    .
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    AOS_SENSOR
''')
for i in global_macros:
    component.add_global_macros(i)



