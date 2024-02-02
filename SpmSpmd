//😄Spm-Spmd实现，CSC矩阵*CSR矩阵，得到Dense Matrix

//CSC * CSR得到Dense矩阵
    Matrix spmspmd(const CSCMatrix& other) const {
        if (cols != other.rows) {
            throw "Matrix dimensions don't match";
        }

        // 结果矩阵
        std::vector<double> resultData;
        std::vector<int> resultColIndices;
        std::vector<int> resultRowPtr(rows + 1, 0);

        Matrix result(rows, other.cols);


        int nnz = 0; // 非零元素个数

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < other.cols; ++j) {
                double temp = 0;
                int pA = rowPtr[i];
                int pB = other.colPtr[j];

                while (pA < rowPtr[i + 1] && pB < other.colPtr[j + 1]) {
                    if (colIndices[pA] < other.rowIndices[pB]) {
                        pA++;
                    }
                    else if (colIndices[pA] > other.rowIndices[pB]) {
                        pB++;
                    }
                    else {
                        temp += data[pA] * other.data[pB];
                        pA++;
                        pB++;
                    }
                }

                if (temp != 0) {
                    result.set(i, j, temp);
                }
            }

           
        }

        return result;
    }
