# Operator
SpMV and Spm-Spm operator in C++

# Matrix类
作用是定义了dense情况下的SPMV和SPM-SPM的算子，作为基础引入

# CSCMatrix 和 SMatrix类

1.作用是定义了sparse情况下的SPMV和SPM-SPM算子

2.其中SPMV我认为**是可以拆分为一个for each**的

SPM-SPM算法我分为了两种情况：两个CSR相乘和一个CSR乘一个CSC；

3.两个CSR的算法（函数为SPM-SPM大写）已经实现，但是我觉得没有办法加for each，因为有一个变量在循环中会被一直访问，从尾部压入数据

**但是如果引入一个同步排序机制（类似ROB），可以保证压入的顺序我觉得可以分为2个for each**

4.CSR和CSC相乘的算法（函数为spm-spm）同上，会有一个变量在循环中会被一直访问，从尾部压入数据
**引入一个同步排序机制（类似ROB）后，可以分为2个for each**


# 数据流图

## SpMV
![image](https://github.com/HPC-SDC/Operator/assets/150303558/ff8f50b9-89be-42c1-9e36-3cb2a47b2872)


## Spm-Spm

