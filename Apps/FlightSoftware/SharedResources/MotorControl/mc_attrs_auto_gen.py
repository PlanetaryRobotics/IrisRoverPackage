# Auto Generator for Source Files for MotorControl Shared Configurations & Attributes

import json

mc_attrs_def_f_json = open('mc_attrs_def.json','r')
mc_attrs = json.load(mc_attrs_def_f_json.read())

mc_gen

mc_attrs_gen_f_hpp = open('mc_config_defs', 'w')

for motor_cnt in mc_attrs["motors"]:
    mc_attrs_gen_f_hpp.write("#define %s ")

mc_attrs_gen_f_hpp.close()
mc_attrs_def_f_json.close()