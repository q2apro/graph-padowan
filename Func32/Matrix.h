/* Copyright 2007 Ivan Johansen
 *
 * Func32 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef MatrixH
#define MatrixH
//---------------------------------------------------------------------------
#include <vector>
#include <valarray>
#include <sstream>
#include <iomanip>

namespace Matrix
{
  //Exception classes
  //EMatrix: Abstarct class used for derivation
  //EIncompatible: Thrown when two matrix' doesn't match the requirements
  //EIndexBound: Thrown when a row or coloumn that doesn't exists was indexed
  //EInvalid: Thrown when trying to compute on a matrix that doesn't match the requirements.
  //ENoResult: Thrown when a computation doesn't give one unambigouos result
  //ELimitation: You reached a limitation in the software, which cannot handle this situation
  class EMatrix : public std::exception
  {
  };

  class EIncompatible : public EMatrix
  {
  public:
    const char* what() const throw()
    {
      return "Incompatible matrix types. The number of rows and coloumns doesn't match the requirements.";
    }
  };

  class EIndexBound : public EMatrix
  {
    std::string Str;
  public:
    enum TIndexType {itRow, itCol};
    EIndexBound(TIndexType Type, unsigned Index, unsigned Size)
    {
      std::stringstream S;
      S << "Tried to index " << (Type==itRow ? "row " : "coloumn ") << Index;
      S << " in matrix with " << Size << (Type==itRow ? " rows." : " coloumns.");
      Str = S.str();
    }
    const char* what() const throw()
    {
      return Str.c_str();
    }
    ~EIndexBound() throw() {}
  };

  class EInvalid : public EMatrix
  {
  public:
    const char* what() const throw()
    {
      return "Invalid operation on matrix. The matrix doesn't match the requirements.";
    }
  };

  class ENoResult : public EMatrix
  {
  public:
    const char* what() const throw()
    {
      return "No unambigouos result found.";
    }
  };

  class ELimitation : public EMatrix
  {
  public:
    const char* what() const throw()
    {
      return "The calculation reached a limitation in the TMatrix class.\nThe software wasn't designed to handle this situation";
    }
  };

  class EDivByZero : public EMatrix
  {
  public:
    const char* what() const throw()
    {
      return "The program tried to divide by zero";
    }
  };


//Template class TMatrix<T> used for handling matrix operation on type T
template<class T> class TMatrix
{
  std::valarray<T> Data;
  unsigned RowCount;
  unsigned ColCount;

  //Helper class used to index a row
  class TRowRef
  {
    friend class TMatrix<T>;
    TMatrix &Matrix;
    unsigned Row;
    TRowRef(TMatrix *M, unsigned ARow) : Matrix(*M), Row(ARow)
    {
    }
    TRowRef(const TRowRef&);
    TRowRef& operator=(const TRowRef&);
  public:
    T& operator[](unsigned Col)
    {
      return Matrix(Row,Col);
    }

    const T& operator[](unsigned Col) const
    {
      return Matrix(Row,Col);
    }
  };

  //Constructor used internally to initialize the matrix with a valarray
  TMatrix(unsigned Rows, unsigned Cols, const std::valarray<T> &A) : RowCount(Rows), ColCount(Cols), Data(A)
  {
  }

  //Returns a reference to the value in the specified row and coloumn; Only boundary check in debug mode
  T& Get(unsigned Row, unsigned Col)
  {
#ifdef _DEBUG
    CheckRow(Row);
    CheckCol(Col);
#endif
    return Data[Row * ColCount + Col];
  }

  //Returns the value in the specified row and coloumn; Only boundary check in debug mode
  T Get(unsigned Row, unsigned Col) const
  {
#ifdef _DEBUG
    CheckRow(Row);
    CheckCol(Col);
#endif
    return Data[Row * ColCount + Col];
  }

  std::slice GetRow(unsigned Row) const
  {
    return std::slice(Row*ColCount, ColCount, 1);
  }

  //Rearranges the matrix to an echelon matrix using Gauss' elimination
  void MakeEchelon()
  {
    for(unsigned i = 0; i < RowCount; i++)
    {
      for(unsigned j = i+1; j < RowCount; j++)
      {
        if(!Get(i,i))
          throw ELimitation();
        T Factor = Get(j,i)/Get(i,i);
        if(Factor)
          for(unsigned k = 0; k < ColCount; k++)
            if(k == i && k < j)
              Get(j,k) = 0;
            else
              Get(j,k) -= Factor*Get(i,k);
      }
    }
  }

  void CheckRow(unsigned Row) const
  {
    if(Row >= RowCount)
      throw EIndexBound(EIndexBound::itRow, Row, RowCount);
  }

  void CheckCol(unsigned Col) const
  {
    if(Col >= ColCount)
      throw EIndexBound(EIndexBound::itCol, Col, ColCount);
  }

public:
  //Constructor creates a matrix with given number of rows and coloumns
  TMatrix(unsigned Rows, unsigned Cols) : Data(0.0, Rows * Cols), RowCount(Rows), ColCount(Cols)
  {
  }

  //Constructs a matrix with given number of rows and coloumns and initializes it with given array
  TMatrix(unsigned Rows, unsigned Cols, T Array[]) : Data(Array, Rows*Cols), RowCount(Rows), ColCount(Cols)
  {
  }

  //Returns the number of coloumns in the matrix
  unsigned GetCols() const
  {
    return ColCount;
  }

  //Returns the number of rows in the matrix
  unsigned GetRows() const
  {
    return RowCount;
  }

  //Returns the element at the given row and coloumn
  T operator()(unsigned Row, unsigned Col) const
  {
    CheckCol(Col);
    CheckRow(Row);
    return Get(Row, Col);
  }

  //Returns a reference to the element at the given row and coloumn
  T& operator()(unsigned Row, unsigned Col)
  {
    CheckCol(Col);
    CheckRow(Row);
    return Get(Row, Col);
  }

  //Returns a helper class with a reference to the row
  const TRowRef operator[](unsigned Row) const
  {
    return TRowRef(const_cast<TMatrix<T>*>(this), Row);;
  }

  //Returns a helper class with a reference to the row
  TRowRef operator[](unsigned Row)
  {
    return TRowRef(this, Row);
  }

  //Unary plus
  TMatrix<T> operator+() const
  {
    return TMatrix<T>(RowCount, ColCount, +Data);
  }

  //Unary minus, negates all values in the matrix
  TMatrix<T> operator-() const
  {
    return TMatrix<T>(RowCount, ColCount, -Data);
  }

  //Multiplies all values in the matrix with Value
  TMatrix<T>& operator*=(const T& Value)
  {
    Data *= Value;
    return *this;
  }

  //Divides all the values in the matrix with Value
  TMatrix<T>& operator/=(const T& Value)
  {
    Data /= Value;
    return *this;
  }

  //Adds the matrix with a another matrix of the same size
  TMatrix<T>& operator+=(const TMatrix<T> &Matrix)
  {
    if(RowCount != Matrix.RowCount || ColCount != Matrix.ColCount)
      throw EIncompatible();
    Data += Matrix.Data;
    return *this;
  }

  //Subtracts antoher matrix of the same size from this matrix
  TMatrix<T>& operator-=(const TMatrix<T> &Matrix)
  {
    if(RowCount != Matrix.RowCount || ColCount != Matrix.ColCount)
      throw EIncompatible();
    Data -= Matrix.Data;
    return *this;
  }

  //Returns the sum of this matrix and another matrix with the same size
  TMatrix operator+(const TMatrix<T> &Matrix) const
  {
    return TMatrix<T>(*this) += Matrix;
  }

  //Returns the difference of this matrix and another matrix of the same size
  TMatrix operator-(const TMatrix<T> &Matrix) const
  {
    return TMatrix<T>(*this) -= Matrix;
  }

  //Returns this matrix multiplied by Value
  TMatrix operator*(const T& Value) const
  {
    return TMatrix<T>(*this) *= Value;
  }

  //Returns this matrix divided by Value
  TMatrix operator/(const T &Value) const
  {
    return TMatrix<T>(*this) /= Value;
  }

  //Returns true if this matrix has the same size and contents as the other matrix
  bool operator==(const TMatrix<T> &Matrix) const
  {
    if(RowCount != Matrix.RowCount || ColCount != Matrix.ColCount)
      return false;
    for(unsigned n = 0; n < Data.size(); n++)
      if(Data[n] != Matrix.Data[n])
        return false;
    return true;
  }

  //Returns true if the two matrixes differs in size or contents
  bool operator!=(const TMatrix<T> &Matrix) const
  {
    return !operator==(Matrix);
  }

  TMatrix<T>& operator/=(const TMatrix<T> &Matrix)
  {
    return *this = Inverse(*this) * Matrix;
  }

  TMatrix<T> operator/(const TMatrix<T> &Matrix) const
  {
    return Inverse(*this) * Matrix;
  }

  void Resize(unsigned Rows, unsigned Cols)
  {
    RowCount = Rows;
    ColCount = Cols;
    Data.resize(Rows*Cols);
  }

  //Multiplies this matrix with another matrix
  //This matrix must have the same number of coloumns as the number of rows in the other matrix.
  //The returned matrix will have the same number of rows as this matrix and the
  //same number of coloumns as the other matrix.
  TMatrix<T> operator*(const TMatrix<T> &Matrix) const
  {
    if(ColCount != Matrix.RowCount)
      throw EIncompatible();
    TMatrix<T> Dest(RowCount, Matrix.ColCount);
    for(unsigned Row = 0; Row < RowCount; Row++)
      for(unsigned Col = 0; Col < Matrix.ColCount; Col++)
      {
        Dest.Get(Row, Col) = 0;
        for(unsigned l = 0; l < ColCount; l++)
          Dest.Get(Row, Col) += Get(Row, l) * Matrix.Get(l, Col);
      }
    return Dest;
  }

  //Multiplies this matrix with a vector
  //This matrix must have the same number of coloumns as the number of rows in the other matrix.
  //The returned matrix will have the same number of rows as this matrix and the
  //same number of coloumns as the other matrix.
  TMatrix<T> operator*(const std::vector<T> &Vector) const
  {
    if(ColCount != Vector.size())
      throw EIncompatible();
    TMatrix<T> Dest(RowCount, 1);
    for(unsigned Row = 0; Row < RowCount; Row++)
    {
      Dest.Get(Row, 0) = 0;
      for(unsigned l = 0; l < ColCount; l++)
        Dest.Get(Row, 0) += Get(Row, l) * Vector[l];
    }
    return Dest;
  }

  //Multiplies this matrix with another matrix.
  //This will change the number of rows and coloumns of the matrix
  TMatrix<T>& operator*=(const TMatrix &Matrix)
  {
    return *this = operator*(Matrix);
  }

  //Swaps the given rows
  void SwapRows(unsigned Row1, unsigned Row2)
  {
    CheckRow(Row1);
    CheckRow(Row2);
    std::swap_ranges(&Data[Row1 * ColCount], &Data[(Row1+1) * ColCount], &Data[Row2 * ColCount]);
  }

  //Swaps the given coloumns
  void SwapCols(unsigned Col1, unsigned Col2, TMatrix<T> &Matrix)
  {
    CheckCol(Col1);
    Matrix.CheckCol(Col2);
    if(Data.size() != Matrix.Data.size())
      throw EIncompatible();
    for(unsigned Row = 0; Row < Data.size(); Row++)
      std::swap(Data[Row][Col1], Matrix.Data[Row][Col2]);
  }

  //Swaps the given coloumns in this
  void SwapCols(unsigned Col1, unsigned Col2)
  {
    SwapCols(Col1, Col2, *this);
  }

  T Determinant() const
  {
    if(ColCount != RowCount)
      throw EInvalid();

    if(ColCount == 1)
      return Data[0];

    TMatrix<T> Matrix(*this);
    Matrix.MakeEchelon();
    T D = Matrix.Get(0,0);
    for(unsigned i = 1; i < ColCount; i++)
      D *= Matrix.Get(i,i);
    return D;
  }

  //Returns the transposition of the matrix
  TMatrix<T> Transpose() const
  {
    TMatrix<T> Matrix(ColCount, RowCount);
    for(unsigned Row = 0; Row < RowCount; Row++)
      for(unsigned Col = 0; Col < ColCount; Col++)
        Matrix.Get(Col, Row) = Get(Row, Col);
    return Matrix;
  }

  //Returns the inverse of the matrix using Gauss-Jordan elimination
  TMatrix<T> Inverse() const;

  //Returns the augmented matrix of this matrix and the argument
  TMatrix<T> Augment(const TMatrix<T> &Matrix) const
  {
    if(RowCount != Matrix.RowCount)
      throw EIncompatible();
    TMatrix<T> M(*this);
    M.Resize(RowCount, ColCount + Matrix.ColCount);

    for(unsigned Row = 0; Row < RowCount; Row++)
      for(unsigned Col = 0; Col < ColCount; Col++)
        M.Get(Row, Col) = Get(Row, Col);

    for(unsigned Row = 0; Row < RowCount; Row++)
      for(unsigned Col = 0; Col < ColCount; Col++)
        M.Get(Row, ColCount + Col) = Matrix.Get(Row, Col);
    return M;
  }

  //Returns the stacked matrix of this matrix and the argument
  TMatrix<T> Stack(const TMatrix<T> &Matrix) const
  {
    if(ColCount != Matrix.ColCount)
      throw EIncompatible();
    TMatrix<T> M(*this);
    M.SetRows(Data.size() + Matrix.Data.size());
    for(unsigned Row = 0; Row < Matrix.Data.size(); Row++)
      for(unsigned Col = 0; Col < ColCount; Col++)
        M.Data[Data.size() + Row][Col] = Matrix.Data[Row][Col];
    return M;
  }

  //This uses Gauss Elimination to solve a lienar system
  //The matrix must contain the values a in a set of linear systems:
  //a11*x1 + a12*x2 + ... + a1n*xn = b1
  //a21*x1 + a22*x2 + ... + a2n*xn = b2
  //...
  //an1*x1 + an2*x2 + ... + ann*xn = bn
  //---                    ---
  //| a11  a12  ...  a1n  b1 |
  //| a21  a22  ...  a2n  b2 |
  //| ...  ...  ...  ...  .. |
  //| an1  an2  ...  ann  bn |
  //---                    ---
  //A vector with the solved unknowns is returned
  std::vector<T> Gauss() const
  {
    //Make sure that the matrix is determined, exactly as many equations as unknowns
    if(RowCount+1 != ColCount)
      throw EInvalid();
    TMatrix<T> M(*this);
    M.MakeEchelon();
    std::vector<T> x(RowCount);
    for(int j = RowCount-1; j >= 0; j--)
    {
      x[j] = M.Get(j, ColCount-1);
      for(unsigned k = j+1; k < RowCount; k++)
        x[j] -= M.Get(j, k) * x[k];
      if(!M.Get(j, j))
        throw ENoResult();
      x[j] /= M.Get(j, j);
    }
    return x;
  }

  TMatrix<T> SubMatrixCols(unsigned FirstCol, unsigned ColNum)
  {
    TMatrix<T> Matrix(RowCount, ColNum);
    for(unsigned Row = 0; Row < RowCount; Row++)
      for(unsigned Col = 0; Col < ColNum; Col++)
        Matrix.Get(Row, Col) = Get(Row, Col + FirstCol);

    return Matrix;
  }
};
//---------------------------------------------------------------------------
template<class T> std::ostream& operator<<(std::ostream &out, const TMatrix<T> &Matrix)
{
  unsigned MaxSize = 0;
  for(unsigned Row = 0; Row < Matrix.GetRows(); Row++)
    for(unsigned Col = 0; Col < Matrix.GetCols(); Col++)
    {
      std::stringstream str;
      str << Matrix(Row, Col);
      if(str.str().size() > MaxSize)
        MaxSize = str.str().size();
    }

  MaxSize += 2;

  out << "---";
  for(unsigned n = 0; n < MaxSize * Matrix.GetCols() - 2; n++)
    out << " ";
  out << "---" << std::endl;
  for(unsigned Row = 0; Row < Matrix.GetRows(); Row++)
  {
    out << "|";
    for(unsigned Col = 0; Col < Matrix.GetCols(); Col++)
      out << std::setw(MaxSize) << Matrix(Row, Col);
    out << "  |" << std::endl;
  }
  out << "---";
  for(unsigned n = 0; n < MaxSize * Matrix.GetCols() - 2; n++)
    out << " ";
  out << "---" << std::endl;
  return out;
}
//---------------------------------------------------------------------------
//This uses Gauss Elimination to solve a lienar system
//The matrix must contain the values a in a set of linear systems:
//a11*x1 + a12*x2 + ... + a1n*xn = b1
//a21*x1 + a22*x2 + ... + a2n*xn = b2
//...
//an1*x1 + an2*x2 + ... + ann*xn = bn
//    ---               ---      -  -
//    | a11  a12  ...  a1n |    | b1 |
//    | a21  a22  ...  a2n |    | b2 |
// A= | ...  ...  ...  ... | B= | .. |
//    | an1  an2  ...  ann |    | bn |
//    ---               ---      -  -
//A vector with the solved unknowns is returned
template<typename T>
std::vector<T> Gauss(const TMatrix<T> &A, const TMatrix<T> &B)
{
  //Make sure that the matrix is determined, exactly as many equations as unknowns
  if(A.GetRows() != A.GetCols() || B.GetRows() != A.GetRows() || B.GetCols() != 1)
    throw EInvalid();

  TMatrix<T> M(A.GetRows(), A.GetCols() + 1);
  for(unsigned Col = 0; Col != A.GetCols(); Col++)
    for(unsigned Row = 0; Row != A.GetRows(); Row++)
      M(Row, Col) = A(Row, Col);

  for(unsigned Row = 0; Row != B.GetRows(); Row++)
    M(Row, M.GetCols() - 1) = B(Row, 0);

  return M.Gauss();
}
//---------------------------------------------------------------------------
//---             ---
//| 1  0  0 ... 0 0 |
//| 0  1  0 ... 0 0 |
//| 0  0  1 ... 0 0 |
//| ............... |
//| 0  0  0 ... 1 0 |
//| 0  0  0 ... 0 1 |
//---             ---
//Returns an identity matrix
template<typename T>
TMatrix<T> IdentityMatrix(unsigned n)
{
  TMatrix<T> Matrix(n, n);
  for(unsigned i = 0; i < n; i++)
    Matrix(i, i) = 1;
  return Matrix;
}
//---------------------------------------------------------------------------
//Returns the inverse of the matrix using Gauss-Jordan elimination
template<typename T>
TMatrix<T> TMatrix<T>::Inverse() const
{
  if(RowCount != ColCount)
    throw EInvalid();

  TMatrix Matrix = Augment(IdentityMatrix<T>(ColCount));

  Matrix.MakeEchelon();

  for(unsigned Row = 0; Row < Matrix.RowCount; Row++)
  {
    if(!Matrix.Get(Row, Row))
      throw EDivByZero();

    T Value = 1 / Matrix.Get(Row, Row);
    for(unsigned Col = 0; Col < Matrix.ColCount; Col++)
      Matrix.Get(Row, Col) *= Value;
  }

  for(int I = RowCount - 1; I >= 0; I--)
  {
    for(int Row = 0; Row < I; Row++)
    {
      T Value = Matrix.Get(Row, I);
      for(unsigned Col = 0; Col < Matrix.ColCount; Col++)
        Matrix.Get(Row, Col) -= Value * Matrix.Get(I, Col);
    }
  }

  return Matrix.SubMatrixCols(ColCount, ColCount);
}
//---------------------------------------------------------------------------
} //namespace Matrix
#endif


