#!/usr/bin/python

import sys
import re

def get_mem_info(map_file):
    total_ram = 0
    total_rom = 0
    map_lines = []
    with open(map_file, 'r') as f:
        s = f.read()

        # find the memory configuration
        mem_config_list = re.findall('Memory Configuration\r?\n\r?\nName             Origin             Length             Attributes\r?\n([\s\S]+)\r?\nLinker script and memory map', s)
        mem_config_text =  '' if not mem_config_list else mem_config_list[0]
        if not mem_config_text:
            print 'Can\'t parse memory configure, memory info get fail!'
            return
        
        region_size_template = {}

        # find the ROM configuration
        rom_config_text = re.findall('(\w+)\s+(0x\w+)\s+(0x\w+)\s+(?:xr|r|axrl !w)\r?\n',mem_config_text)
        # get every ROM configuration's start - end address
        rom_config = []
        for rom in rom_config_text:
            rom_config += [{'start':int(rom[1], 16), 'end':int(rom[1], 16) + int(rom[2], 16), 'name':rom[0]}]
            region_size_template[rom[0]] = 0
        
        # find the RAM configuration
        ram_config_text = re.findall('(\w+)\s+(0x\w+)\s+(0x\w+)\s+(?:xrw|rw|axw)\r?\n',mem_config_text)
        if (len(ram_config_text)+len(rom_config_text)) == 0:
            ram_config_text = re.findall('\*default\*\s+(0x\w+)\s+(0x\w+)\r?\n',mem_config_text)
            print ('no definite address hint,using default mem configuration')

        # get every RAM configuration's  start - end address
        ram_config = []
        for ram in ram_config_text:
            ram_config += [{'start':int(ram[1], 16), 'end':int(ram[1], 16) + int(ram[2], 16), 'name':ram[0]}]
            region_size_template[ram[0]] = 0

        # find memory map (without discard and debug sections)
        mem_map_list = re.findall('Linker script and memory map([\s\S]+?)OUTPUT\('+map_file.replace('.map','.elf'), s)
        mem_map = '' if not mem_map_list else mem_map_list[0]
        if not mem_map:
            print 'Can\'t parse memory info, memory info get fail!'
            return

        mem_map = mem_map.replace('\r', '')
        modules = []
        for l in mem_map.split('\n'):
            m = re.search('0x\w+\s+0x\w+\s+.+?([^/\\\]+\.[ao])\((.+\.o)\)', l)
            if m == None:
                continue
            modules.append(m.groups()[0])
        modules = list(set(modules))

        # find sections address - length in memory map
        # modules = list(set(item[0] for item in re.findall('0x\w+\s+0x\w+\s+.+?([^/\\\]+\.[ao])(\(.+\.o\))?\r?\n', mem_map)))
        modules.sort(key = lambda x : x.upper())
        modules += ['*fill*']
        
        region_size = region_size_template.copy()
        
        for module in modules:
            rom_size = 0
            ram_size = 0
            region_size_module = region_size_template.copy()
            module = module.replace('+', '\+')
            # get module's sections's address and size
            if(module == '*fill*'):
                sections = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall('\*fill\*[ \t]+(0x\w+)[ \t]+(0x\w+)[ \t]+\r?\n', mem_map))
            else:
                sections = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall('(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\\]'+module+'(\(.+\.o\))?\r?\n', mem_map))
            if(not sections):
                continue

            def find_region(addr, config):
                for info in config:
                    if(info['start'] <= addr < info['end']):
                        return info['name']
                return None
                        
            for section in sections:
                is_ram = 1;
                region_name = find_region(section['address'], ram_config)
                if(region_name is None):
                    is_ram = 0;
                    region_name = find_region(section['address'], rom_config)
                
                if(region_name is None):
                    continue

                if (is_ram):
                    ram_size += section['size']
                else:
                    rom_size += section['size']
                
                region_size_module[region_name] += section['size']
                region_size[region_name] += section['size']

            total_ram += ram_size
            total_rom += rom_size

            map_lines.append('| %-40s | %-8d  | %-8d | %-8d | %-8d | %-8d | %-8d |'%(re.sub('\.[ao]','',module)[:40],rom_size,ram_size, region_size_module['rom'], region_size_module['flash'], region_size_module['ram_tcm'], region_size_module['ram_wifi']))

    print '\n                                             AOS MEMORY MAP                                                  '
    print '|=============================================================================================================|'
    print '| %-40s | %-8s  | %-8s | %-8s | %-8s | %-8s | %-8s |'%('MODULE','ROM','RAM','rom','flash','ram_tcm', 'ram_wifi')
    print '|=============================================================================================================|'
    for line in map_lines:
        print line
    print '|=============================================================================================================|'
    print '| %-40s | %-8d  | %-8d | %-8d | %-8d | %-8d | %-8d |'%('TOTAL (bytes)', total_rom, total_ram, region_size['rom'], region_size['flash'], region_size['ram_tcm'], region_size['ram_wifi'])
    print '|=============================================================================================================|'

def main():
    get_mem_info(sys.argv[1])

if __name__ == "__main__":
    main()
