# This is a sample Python script.

# Press ⌃R to execute it or replace it with your code.
# Press Double ⇧ to search everywhere for classes, files, tool windows, actions, and settings.

import os
import sys
from glob import glob
import shutil

#要先处理掉 #include <string> 这种宏，再进行宏展开
#生成临时文件返回
def dropIncludePreMacro(file, tmp_dir, extension, oesTex):
    (path, filename) = os.path.split(file)
    tfilename = '{}/t_{}'.format(tmp_dir, filename)
    # print(tfilename)

    name_key = filename.split('.')[0]
    print('***********=========> dropIncludePreMacro file_name={}, save_file={}'.format(filename, name_key))

    tf = open(tfilename,'w')
    with open(file, 'r') as i_file:
        while line := i_file.readline():
            idx = line.find('//')
            if line.find('#include') != -1 or line.find('#version 300 es') != -1 or idx == 0:
                continue

            ##将扩展保存
            if line.find('#extension') != -1:
                extension[name_key] = line
                continue

            ##oes 纹理存储替换
#             print('---------------------------------------------')
            if line.find('samplerExternalOES') != -1:
                splitArr = line.split(' ')
                #获取tex名称,最后一个元素
                name = splitArr[len(splitArr)-1].replace(';','').replace('\n','').replace('\r','').lstrip()
                if name_key not in oesTex:
                    oesTex[name_key] = dict()
                oesTex[name_key][name] = line
                print('~~~~~~~~~~~~~~~ oseTex ==> filename={}, name={}, line={}'.format(filename, name, line))
                line = line.replace('samplerExternalOES', 'sampler2D')
                print(oesTex)

            # 删除注释【影响字符】
            if idx != -1:
                line = line[0:idx-1]
                # print("@@@@@@@@ ==> {}".format(line))

            if line.find('#') != -1:
                tf.writelines([line])
            else:
                needEnter = False
                if line.find('R"(') != -1:
                    tline = line.replace('R"(','"')
                    line = tline
                if line.find(')";') != -1:
                    tline = line.replace(')";','";')
                    line = tline
                    needEnter = True #此种情况下，不进行换行
                if needEnter == False:
                    tlin = line.replace('\n','').replace('\r','').lstrip()
                    tf.writelines([tlin])
                else:
                    tf.writelines([line])
    tf.close()
    return tfilename


def preCompile(tmp_dir, input_dir, extension, oesTex):
    # 历目录，调用clang++ 预编译c++头文件
    paths = glob('{}/*.hpp'.format(input_dir))
    for file in paths:
        # drop shader_header.hpp
        if file.find('shader_header') == -1:
            tfile = dropIncludePreMacro(file, tmp_dir, extension, oesTex)
            (path, filename) = os.path.split(file)
            os.system('clang++ -E {} -o {}'.format(tfile, '{}/{}.i'.format(tmp_dir, filename)))
            # os.remove(tfile)

def parseShaderString(file, dic):
    # 读文件，解析字符串
    f = open(file)
    line = f.readline()
    while line:
        line = f.readline()
        eq_index = line.find('=')
        if eq_index == -1:
            continue

        #shader name
        eq_left_str = line[0:eq_index-1]
        nameArr = eq_left_str.split(' ')

        shader_name = nameArr[len(nameArr)-1]
        # print(shader_name)

        shader_context = line[eq_index+1:len(line)-2]
        # print(shader_context)
        dic[shader_name] = shader_context
    f.close()


def build_vert(key, value, tmp_dir, shader_type):
    if key.find('vert') != -1:
        # 写入临时文件
        fpath = '{}/{}.vert'.format(tmp_dir, key)
        fw = open(fpath, 'w')
        fw.writelines('#version 310 es\n')

        #替换掉"引号
        replase_value = value.replace('"', '')
        # print(replase_value)
        fw.write(replase_value)
        fw.close()

        # 进行glslang编译成spv文件
        print("     ***************>>> glslang begin...")
        os.system('./tools/glslang -G -S vert {} -o {}.spv'.format(fpath, fpath))
        # 在通过spirv-cross 编译成msl
        print("     ***************>>> glslang end...")

        if shader_type == 'metal':
            msl_name = '{}/{}.metal.metal'.format(tmp_dir, key)
            print('     ***************>>> entry-function:{}'.format(msl_name))
            # metal_merge_files.append(msl_name)
            # metal_merge_file_name = key
            os.system('./tools/spirv-cross --msl --msl-ios --iterations 1  --rename-entry-point main {} vert {}.spv --output {}'.format(key, fpath, msl_name))
            return msl_name
        elif shader_type == 'es200':
            msl_name = '{}/{}.gl200.vert'.format(tmp_dir, key)
            # metal_merge_files.append(msl_name)
            metal_merge_file_name = key
            os.system('./tools/spirv-cross --es --version 200 {}.spv --output {}'.format(fpath, msl_name))
            return msl_name
        elif shader_type == 'es300':
            msl_name = '{}/{}.gl300.vert'.format(tmp_dir, key)
            # metal_merge_files.append(msl_name)
            metal_merge_file_name = key
            os.system('./tools/spirv-cross --es --version 300 {}.spv --output {}'.format(fpath, msl_name))
            return msl_name

def build_frag(key, value, tmp_dir, shader_type):
    if key.find('frag') != -1:
        # 写入临时文件
        fpath = '{}/{}.frag'.format(tmp_dir, key)
        fw = open(fpath, 'w')
        fw.writelines('#version 310 es\n')

        # 替换掉"引号
        replase_value = value.replace('"', '')
        # print(replase_value)
        fw.write(replase_value)
        fw.close()

        # 进行glslang编译成spv文件
        print("     ***************>>> glslang begin...")
        os.system('./tools/glslang -G -S frag {} -o {}.spv'.format(fpath, fpath))
        print("     ***************>>> glslang end...")
        # 在通过spirv-cross 编译成msl

        if shader_type == 'metal':
            msl_name = '{}/{}.metal.metal'.format(tmp_dir, key)
            # print('{},{}'.format(key, msl_name))
            # metal_merge_files.append(msl_name)
            # metal_merge_file_name = '{}_{}.metal'.format(metal_merge_file_name, key)
            print('     ***************>>> entry-function:{}'.format(msl_name))
            os.system('./tools/spirv-cross --msl --msl-ios --iterations 1 --rename-entry-point main {} frag {}.spv --output {}'.format(key, fpath, msl_name))
            return msl_name
        elif shader_type == 'es200':
            msl_name = '{}/{}.gl200.frag'.format(tmp_dir, key)
            print('>>>>>{},{}'.format(key, msl_name))
            # metal_merge_files.append(msl_name)
            # metal_merge_file_name = '{}_{}.gl200'.format(metal_merge_file_name, key)
            os.system('./tools/spirv-cross --es --version 200 {}.spv --output {}'.format(fpath, msl_name))
            return msl_name
        elif shader_type == 'es300':
            msl_name = '{}/{}.gl300.frag'.format(tmp_dir, key)
            print('<<<<<{},{}'.format(key, msl_name))
            # metal_merge_files.append(msl_name)
            # metal_merge_file_name = '{}_{}.gl200'.format(metal_merge_file_name, key)
            os.system('./tools/spirv-cross --es --version 300 {}.spv --output {}'.format(fpath, msl_name))
            return msl_name

def mergeAndSaveMetalFile(file, output_dir, merge_files, ext):
    #合并文件
    (path, save_file) = os.path.split(file)
    save_file_name = save_file.split('.')[0]
    merge_file_path = '{}/{}.{}'.format(output_dir, save_file_name, ext)
    if os.path.exists(output_dir) == False:
        os.makedirs(output_dir)

    print('     **************>>>> save file ==> {}'.format(merge_file_path))
    s_file = open(merge_file_path, 'w')
    idx = 0
    for key in merge_files:
        if key == None:
            continue

        t_name = key
        print('         ****************>>>> file: {}'.format(t_name))
        tf = open(t_name, 'r')
        #去除前面几行 import
        if(idx > 0):
            i = 0
            while i < 4:
                _ = tf.readline()
                i += 1
        ctx = tf.read()
        s_file.write(ctx)
        # metal_file.writelines()
        tf.close()
        idx += 1
    s_file.close()


def mergeAndSaveGLESFile(file, output_dir, merge_files, ext, extension, oesTex):
    #合并文件
    (path, save_file) = os.path.split(file)
    save_file_name = save_file.split('.')[0]
    merge_file_path = '{}/{}.hpp'.format(output_dir, save_file_name, ext)


    print('***********=========> mergeAndSaveGLESFile file_name={}, save_file={}'.format(save_file_name, merge_file_path))

    if os.path.exists(output_dir) == False:
        # shutil.rmtree(output_dir)
        os.makedirs(output_dir)
        print('+++++++ {}'.format(output_dir))
    metal_file = open(merge_file_path, encoding='utf-8', mode = 'w')

    # 宏定义
    macro_name0 = '#ifndef {}_{}_HPP\n'.format(save_file_name.upper(), ext.upper())
    macro_name1 = '#define {}_{}_HPP\n'.format(save_file_name.upper(), ext.upper())
    macro_name2 = '#endif'

    #include
    include_file = '#include <string>\n\n\n'

    metal_file.writelines([macro_name0, macro_name1, include_file])

    for key in merge_files:
        if key == None:
            continue

        t_name = key
        tf = open(t_name)
        (path, filename) = os.path.split(key)
        mark_name = filename.split('.')[0]
        print('         ***{}'.format(mark_name))
        #c++ 字符串


        cpp_str = 'static const std::string {} = R"('.format(mark_name)
        # print(cpp_str)
        #写变量名
        metal_file.write(cpp_str)

        if mark_name.find('_frag') != -1:
            if save_file_name in extension:
                metal_file.writelines("#version 300 es\n\r")
                metal_file.writelines(extension[save_file_name])
                ctx = tf.readline()

        ctx = tf.readline()
        while ctx:

            ##如果是纹理
            if ctx.find("sampler2D") != -1 and save_file_name in oesTex:
                #获取name， 查询oesTex
                arrSP = ctx.split(' ')
                name = arrSP[len(arrSP) - 1].replace(';','').replace('\n','').replace('\r','').lstrip()
                if name in oesTex[save_file_name]:
                    ctx = oesTex[save_file_name][name]

                print('~~~~~~~~~~~~~!!!!! {}'.format(name))
                print(arrSP)
                print(ctx)

            metal_file.write(ctx)
            ctx = tf.readline()
        metal_file.write(')";\n\r')
        tf.close()

    metal_file.writelines([macro_name2])
    metal_file.close()

def build_shader_common(tmp_dir, input_dir, output_dir, ext):
    print('1 ******************* build_shader--------')

    extension = dict()
    oesTex = dict()
    preCompile(tmp_dir, input_dir, extension, oesTex)

    #2. 遍历目录，提取shader字符串
    print(" ******************** trace file dir")
    paths = glob(r'{}/*.hpp.i'.format(tmp_dir))
    for file in paths:
        print('     **********>>>> read file ==> {}'.format(file))
        dic = dict()
        # 解析shader
        parseShaderString(file, dic)
        # 写入转成metal文件
        #字符串名字 -- 映射为metal的函数名， 要求字符串名 要以 vert / frag 结尾
        merge_files = []
        for key, value in dic.items():
            v_name = build_vert(key, value, tmp_dir, ext)
            merge_files.append(v_name)

            f_name = build_frag(key, value, tmp_dir, ext)
            merge_files.append(f_name)
    
        print(merge_files)

        if ext == 'metal':
            mergeAndSaveMetalFile(file, output_dir, merge_files, 'metal')
        else:
            mergeAndSaveGLESFile(file, output_dir, merge_files, ext, extension, oesTex)

def build_vulkan_shader(tmp_dir, input_dir, output_dir):
    print('build_vulkan_shader--------')
    print('not supported')

# -msl      指定生成metal shader
# -es300  指定生成gles 300 shader
# -es200  指定生成gles 200 shader
# -vk       指定生成Vulkan shader
# -input    输入目录
# -output   输出目录
if __name__ == '__main__':

    print('begin ******************>>> arg0={}, arg1={}, arg2={}, arg3={}'.format(sys.argv[0],sys.argv[1],sys.argv[2],sys.argv[3]))

    # 临时目录
    tmp_dir = 'tmp_dir'
    if os.path.exists(tmp_dir) == False:
        # shutil.rmtree(tmp_dir)
        os.makedirs(tmp_dir)

    #shader编译
    if sys.argv[1].find('msl') != -1:
        build_shader_common('./{}'.format(tmp_dir), sys.argv[2], sys.argv[3], 'metal')
    elif sys.argv[1].find('es200') != -1:
        build_shader_common('./{}'.format(tmp_dir), sys.argv[2], sys.argv[3], 'es200')
    elif sys.argv[1].find('es300') != -1:
        build_shader_common('./{}'.format(tmp_dir), sys.argv[2], sys.argv[3], 'es300')
    else:
        build_vulkan_shader('./{}'.format(tmp_dir), sys.argv[2], sys.argv[3])

    # 删除临时目录
    shutil.rmtree(tmp_dir)