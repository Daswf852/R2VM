# R2VM  
It's what the repo description says.  
[Manual](https://lbphacker.pw/powdertoy/R216/manual.md)  
## How to compile and run  
`wget https://github.com/LBPHacker/R216/raw/master/r2asm.lua`  
`./compile.sh`  
`cd peripherals && ./compile.sh && cd..`  
`lua r2asm.lua headless_model=R216DVM headless_out=a.bin <assembly>`  
`./r2vm a.bin ports`  
![works on my machine](https://johan.driessen.se/images/johan_driessen_se/WindowsLiveWriter/PersistanceinWF4beta2_E4AD/works-on-my-machine-starburst_2.png)  
### TODO  
 - RECV, WAIT and proper R2TERM
