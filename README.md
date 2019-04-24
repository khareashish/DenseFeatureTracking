# README 



## What is This Repository for? 

* SDSoC project for Dense Feature Tracking based on the [paper](https://hal.archives-ouvertes.fr/hal-01657267/document) by Gonzalez et al. 


## Software Tools and System Requirements 

### Hardware 

* [ZCU102 Evaluation Board](https://www.xilinx.com/products/boards-and-kits/ek-u1-zcu102-g.html) form Xilinx 
    * rev 1.0 with ES2 silicon **or**
    * rev 1.0 with production silicon

### Software

* [SDSoC Development Environment](https://www.xilinx.com/products/design-tools/software-zone/sdsoc.html) version 2018.2 
* Revision Platform zip files for ZCU102:
  * ZCU102 ES2 silicon: [zcu102-es2-rv-ss-2018-2.zip](https://www.xilinx.com/member/forms/download/design-license-xef.html?akdm=1&filename=zcu102-es2-rv-ss-2018-2.zip)
  * ZCU102 Production silicon: [zcu102-rv-ss-2018-2.zip](https://www.xilinx.com/member/forms/download/design-license-xef.html?akdm=1&filename=zcu102-rv-ss-2018-2.zip)


### Licensing

* You will need only the SDSoC license to build the design. 


## How do I get set up? 

* Install SDx 2018.2 with SDSoC license. You can follow the installation guide [UG1294](https://china.xilinx.com/support/documentation/sw_manuals/xilinx2018_2/ug1294-sdsoc-rnil.pdf)
* Setup Revision Platform : 
    * Extract the contents of the Revision Platform to the SDx installation folder - ..\Xilinx\SDx\2018.2\platforms
    * In Windows: Create an environment variable SYSROOT with value - ..\Xilinx\SDx\2018.2\platforms\zcu102-es2-rv-ss-2018-2\petalinux\sdk\sysroots\aarch64-xilinx-linux  
* Clone this repository to SDSoC workspace and import project into SDSoC environment 
* Open Application project settings by selecting project.sdx in the Project Explorer 
* Make sure that you have the correct revision platform selected (ES2 or production) depending on which board you have  
* Select Active build config - Debug/Release - Hit Build !! 



## Questions ?? 

* Drop an email to Ashish Khare (khare.ashish.90@gmail.com) 
