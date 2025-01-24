# h2xml

## Introduction
H2XML (Header to XML) is a generic tool for generating XML files from annotated
C header files with Grammar and syntax of the annotations are similar to
Doxygen.

## Required tools
- Flex
- Bison

Linux
* Above tools are part of default Image. Use below commands to
check for flex and bison tool:
    ```
    bison --version
    flex --version
    ```
* h2xml is validated with flex v2.6.4 and bison (GNU Bison) v3.8.2 on Ubuntu 22.04.

## Generate h2xml binary

1. Clone h2xml repository

    ```
    git clone https://github.com/Audioreach/h2xml.git
    ```

2. Generate h2xml binary

    Linux
    ```
    cd <h2xml root>/build/linux/
    make debug
    ```
    Above command generates h2xml binary under
    <h2xml root>/build/linux/bin/linux/debug/ directory.

    Use `make clean` to remove generated output and start over.

## Usage

H2XML is a command line based utility. To run h2xml use below command:
```
h2xml –conf <config_file> -o <output_file> [optional parameters] input_file
```

**Mandatory Parameters**

* **-conf <config_file>**: <config_file>  must be a valid h2xml configuration file

    Example config file: [h2xml_config_ar.xml](./config_files/h2xml_config_ar/h2xml_config_ar.xml)

* **<input_file> or – i <input_file>** : A C-header input file with extension
‘.h’. Output file name will be <input_file>.xml. Must be last argument if ‘- i’
is omitted.

* **-t configType** : Include sections in config file tagged with
<H2XML_TYPE name="configType">. Several config types can be specified.

    Available types are: spfProp, driverProp, property_All, Key, spfModule,
    DriverModule, Module_All

To know about optional parameters, run below command:

```
<Directory Path to h2xml binary>/h2xml -h
```

## License

h2xml is licensed under the BSD-3-Clause. Check out the [LICENSE](LICENSE) for more details.
