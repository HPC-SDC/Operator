#include <iostream>
#include <vector>

using namespace std;

class Matrix {
public:
    int rows; // 行数
    int cols; // 列数
    vector<vector<double>> data; // 矩阵数据


    // 构造函数
    Matrix(int _rows, int _cols) {
        rows = _rows;
        cols = _cols;
        data.resize(rows);
        for (int i = 0; i < rows; i++) {
            data[i].resize(cols);
        }
    }

    // 获取行数
    int getRows() const {
        return rows;
    }

    // 获取列数
    int getCols() const {
        return cols;
    }

    // 获取元素值
    double get(int i, int j) const {
        return data[i][j];
    }

    // 设置元素值
    void set(int i, int j, double value) {
        data[i][j] = value;
    }

    // 矩阵向量乘法
    vector<double> SPMV(const vector<double>& v) const {
        if (v.size() != cols) {
            throw "Vector size doesn't match matrix size";
        }

        vector<double> result(rows, 0.0);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result[i] += data[i][j] * v[j];
            }
        }

        return result;
    }

    Matrix SPMSPM(const Matrix& other) const {
        if (cols != other.rows) {
            throw "Matrix dimensions don't match";
        }

        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < other.cols; j++) {
                double sum = 0.0;
                for (int k = 0; k < cols; k++) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.set(i, j, sum);
            }
        }

        return result;
    }
};
class CSCMatrix {
public:
    int rows; // 行数
    int cols; // 列数
    std::vector<double> data; // 非零元素数据
    std::vector<int> rowIndices; // 行索引数组
    std::vector<int> colPtr; // 列指针数组


    // 构造函数
    CSCMatrix(int _rows, int _cols, const std::vector<double>& _data,
        const std::vector<int>& _rowIndices, const std::vector<int>& _colPtr)
        : rows(_rows), cols(_cols), data(_data), rowIndices(_rowIndices), colPtr(_colPtr) {
        if (data.size() != rowIndices.size()) {
            throw "Data size doesn't match row indices size";
        }
        if (_colPtr.size() != cols + 1) {
            throw "Column pointer size doesn't match number of columns";
        }
    }

    // 获取非零元素值
    double getData(int i) const {
        return data[i];
    }

    // 获取行索引值
    int getRowIndex(int i) const {
        return rowIndices[i];
    }

    // 获取列指针值
    int getColPtr(int i) const {
        return colPtr[i];
    }
};

class SMatrix {
    //以CSR形式存储的
public:
    int rows; // 行数
    int cols; // 列数
    vector<double> data; // 非零元素数据
    vector<int> colIndices; // 列索引数组
    vector<int> rowPtr; // 行指针数组


    // 构造函数
    SMatrix(int _rows, int _cols, const vector<double>& _data,
        const vector<int>& _colIndices, const vector<int>& _rowPtr)
        : rows(_rows),cols(_cols), data(_data), colIndices(_colIndices), rowPtr(_rowPtr) {
        if (data.size() != colIndices.size()) {
            //throw "Data size doesn't match column indices size";
        }
        if (_rowPtr.size() != rows + 1) {
            //throw "Row pointer size doesn't match number of rows";
        }
    }

    // 获取非零元素值
    double getData(int i) const {
        return data[i];
    }

    // 获取列索引值
    int getColIndex(int i) const {
        return colIndices[i];
    }

    // 获取行指针值
    int getRowPtr(int i) const {
        return rowPtr[i];
    }

    // 稀疏矩阵向量乘法
    // 注意：此处实现的是Ax，向量右乘；若想实现向量左乘，用CSC存储会更方便
    vector<double> SPMV(const vector<double>& v) const {
        if (v.size() != cols) {
            throw "Vector size doesn't match matrix size";
        }

        vector<double> result(rows, 0.0);
        for (int i = 0; i < rows; i++) {//每一个for之间是独立的，可以用for each

            // a = rowPtr[i];  b=rowPtr[i+1];并行
            for (int j = rowPtr[i]; j < rowPtr[i + 1]; j++) {//每个for之间要用到上一个for涉及到的result

                //temp=colIndices[j];
                //result[i] += data[j] * v[temp];两个顺序执行
                result[i] += data[j] * v[colIndices[j]];
            }
        }

        return result;
    }

    SMatrix SPMSPM(const SMatrix& other) const {
        // 检查矩阵维度是否匹配
        if (cols != other.rows) {
            throw "Matrix dimensions don't match"; // 抛出异常，矩阵维度不匹配
        }

        // 创建结果矩阵对象，初始化为空矩阵
        SMatrix result(rows, other.cols, {}, {}, {});
        vector<int> colPtr(other.cols, -1); // 列指针数组，初始值为-1
        vector<double> resultData; // 存储结果矩阵的非零元素值

        // 遍历当前矩阵的行
        for (int i = 0; i < rows; i++) {//因为resultdata在每个最内层循环里面在尾部添加元素，无法进行for each

            vector<bool> visited(other.cols, false); // 标记其他矩阵列是否被访问过

            // 遍历当前行的非零元素
            for (int j = rowPtr[i]; j < rowPtr[i + 1]; j++) {
                int colIndexA = colIndices[j]; // 当前矩阵列索引
                double valueA = data[j]; // 当前矩阵元素值

                // 遍历与当前列相关的其他矩阵的非零元素
                for (int k = other.rowPtr[colIndexA]; k < other.rowPtr[colIndexA + 1]; k++) {
                    int colIndexB = other.colIndices[k]; // 其他矩阵列索引
                    double valueB = other.data[k]; // 其他矩阵元素值

                    // 如果该列还未访问过
                    if (!visited[colIndexB]) {
                        visited[colIndexB] = true; // 标记该列已访问
                        //新添加一个数据的时候，记录下该数据在resultData里面的位置（因为是在尾巴添加，所以size就是位置）
                        //这个位置由colPtr这个数组记录，即对应列下的部分和被存到了resultData的什么地方
                        //只用存列是因为最后只有B的列输出的列
                        colPtr[colIndexB] = resultData.size(); // 更新列指针数组中的索引位置
                        resultData.push_back(valueA * valueB); // 添加计算结果到结果数据中
                    }
                    else {
                        // 如果该列已经访问过，累加计算结果
                        resultData[colPtr[colIndexB]] += valueA * valueB;
                    }
                }
            }
        }

        vector<int> resultColIndices; // 结果矩阵的非零元素列索引
        vector<int> resultRowPtr = { 0 }; // 结果矩阵行偏移数组，初始值为0

        // 遍历结果矩阵的列
        for (int i = 0; i < other.cols; i++) {
            // 如果该列存在非零元素
            if (colPtr[i] != -1) {
                resultColIndices.push_back(i); // 将列索引添加到结果矩阵的列索引数组中
                resultRowPtr.push_back(resultData.size()); // 添加结果数据的大小到行偏移数组中
            }
        }

        // 构造并返回结果矩阵对象
        return SMatrix(rows, other.cols, resultData, resultColIndices, resultRowPtr);
    }

    SMatrix spmspm(const CSCMatrix& other) const {
        if (cols != other.rows) {
            throw "Matrix dimensions don't match";
        }

        // 结果矩阵
        std::vector<double> resultData;
        std::vector<int> resultColIndices;
        std::vector<int> resultRowPtr(rows + 1, 0);

        int nnz = 0; // 非零元素个数

        for (int i = 0; i < rows; ++i) {
            std::vector<double> temp(other.cols, 0.0); // 临时存储每一行与B相乘的结果

            for (int pA = rowPtr[i]; pA < rowPtr[i + 1]; ++pA) {
                int colA = colIndices[pA];
                double valueA = data[pA];

                for (int pB = other.colPtr[colA]; pB < other.colPtr[colA + 1]; ++pB) {
                    int rowB = other.rowIndices[pB];
                    double valueB = other.data[pB];
                    temp[rowB] += valueA * valueB; // 累加乘积结果
      
                }
            }

            for (int j = 0; j < other.cols; ++j) {
                if (temp[j] != 0.0) {
                    resultData.push_back(temp[j]); // 存储乘积结果中的非零元素值
                    resultColIndices.push_back(j); // 存储乘积结果中的列索引
                    nnz++; // 非零元素数量加一
                }
            }
            resultRowPtr[i + 1] = nnz; // 更新结果矩阵行指针数组的值
        }

        return SMatrix(rows, other.cols, resultData, resultColIndices, resultRowPtr);
    }


};

#include <vector>







int main() {

    vector<double> data = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    vector<int> colIndices = { 0, 2, 1, 0, 2 };
    vector<int> rowPtr = { 0, 2, 3, 5 };

    SMatrix SparseM(3, 3, data, colIndices, rowPtr);

    //向量
    vector<double> b = { 1.0, 2.0, 3.0 };

    // 计算矩阵向量乘积,SpMV
    vector<double> result = SparseM.SPMV(b);

    for (int i = 0; i < result.size(); i++) {
        cout << result[i] << " ";
    }
    cout << endl;


    vector<double> data2 = { 1,4,3,2,5 };
    vector<int> rowIndices = { 0, 2, 1, 0, 2 };
    vector<int> colPtr = { 0, 2, 3, 5 };

    SMatrix SparseM2(3, 3, data, colIndices, rowPtr);

    //计算两个CSR矩阵的Spm-Spm
    SMatrix R = SparseM.SPMSPM(SparseM2);

    CSCMatrix SparseM3(3, 3, data2, rowIndices, colPtr);

    SMatrix R2 = SparseM.spmspm(SparseM3);






 //---------------------------------------------------------------------------
    //非稀疏的SPMV和SPM-SPM
    // 创建一个3x3的矩阵
    Matrix m(3, 3);

    // 设置矩阵元素值
    m.set(0, 0, 1.0);
    m.set(0, 1, 2.0);
    m.set(0, 2, 3.0);
    m.set(1, 0, 4.0);
    m.set(1, 1, 5.0);
    m.set(1, 2, 6.0);
    m.set(2, 0, 7.0);
    m.set(2, 1, 8.0);
    m.set(2, 2, 9.0);

    Matrix m2 = m;

    Matrix a = m.SPMSPM(m2);

    // 创建一个向量
    vector<double> b2 = { 1.0, 2.0, 3.0 };

    // 计算矩阵向量乘积
    result = m.SPMV(b2);

    // 输出结果
    for (int i = 0; i < result.size(); i++) {
        cout << result[i] << " ";
    }
    cout << endl;

    return 0;
}
