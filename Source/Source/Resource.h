/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef ResourceH
#define ResourceH

#define RES_ERROR        1
#define RES_SHADE        2
#define RES_ROTATION     3
#define RES_VERSION      4
#define RES_BUILD        5
#define RES_ERROR_NUMBER 6
#define RES_AREA         7
#define RES_LENGTH       8
#define RES_SERIES       9
#define RES_DEGREES			10
#define RES_LABEL       11
#define RES_UNCERTAINTY 12
#define RES_TRANSPARENT 13
#define RES_AXES        14
#define RES_KEY_CTRL    15
#define RES_KEY_ALT     16
#define RES_KEY_SHIFT   17
#define RES_KEY_DEL     18
#define RES_KEY_INS     19
#define RES_KEY_HOME    20
#define RES_KEY_PGUP    21
#define RES_KEY_PGDN    22
#define RES_KEY_END     23
#define RES_KEY_ESC     24
#define RES_KEY_ENTER   25
#define RES_KEY_SPACE   26
#define RES_KEY_BKSP    27
#define RES_KEY_TAB     28
#define RES_KEY_LEFT    29
#define RES_KEY_RIGHT   30
#define RES_KEY_UP      31
#define RES_KEY_DOWN    32

#define RES_PowCalcError        101 //Error while power raising
#define RES_TanError            102 //Tangent to 90°+p180°
#define RES_FactError           103 //fact must be a positive integer
#define RES_LogError            104 //Logarihm to number equal or less than zero
#define RES_SqrtError           105 //Sqrt on a negative number'
#define RES_ComplexError        106 //Some error ocoured because the value had an imaginary part
#define RES_DivByZero           107 //Division by zero
#define RES_ArcError            108 //Inverse trogonometric function out of range [-1;1]
#define RES_NotDefError         109 //Position not defined; returnbed by Sign(x) or U(x) if x=0
#define RES_ATanhError          110 //atanh(x) at undefined value
#define RES_ACoshError          111 //acosh(x) at undefined value
#define RES_ArgError            112 //arg(0) is undefined
#define RES_EvalFailed          113 //Evaluation failed
#define RES_TotalLoss           114 //Argument would produce function result with total loss of significant digits, such as sin(10e70)
#define RES_SymbolNotFound      115 //Symbol does not exist; A custom function/constant is not updated
#define RES_RecusionLimit       116 //Recursive function has been called too many times
#define RES_HugeValReturned     117 //Overflowe: A function result was too large, e.g. sinh(20000)
#define RES_ExtFuncError        118 //A call to an external custom function failed.

#define RES_OperatorError       150 //Operator cannot be placed here. Two operators may not follow each other
#define RES_NoEndPar            155 //End of parenthese missing
#define RES_ArgCountError       156 //Invalid number of arguments
#define RES_InvalidCompare      157 //Compare operator misplaced. Only two compare operators allowed. "5<x<y<10" will give the error.
#define RES_InvalidNumber       158 //Number expected
#define RES_EmptyString         159 //String is empty
#define RES_CommaError          160 //Comma not allowed here; Use dot as decimal separator
#define RES_InvalidEndPar       161 //Unexpected end of parenthese; Example: sin();  x^)
#define RES_FactorExpected      163 //A factor (Number, constant or function) was expected
#define RES_ParAfterConst       164 //Parameter after constant or variable not allowed
#define RES_ExpressionExp       165 //Expression expected
#define RES_UnknownVar          166 //Unknown variable found
#define RES_UnknownChar         167 //Unknown character found
#define RES_UnexpectedEnd       168 //The end of the expresion was unexpected; Something else was expected here
#define RES_ParseError          170 //Error parsing function text; Exact cause unknown

#define RES_Overflow            171 //A calculation resultet in an overflow
#define RES_InvalidValue        173 //An invalid value was used in the calculation
#define RES_TooFewPoints        174 //Not enough points for calculation
#define RES_IllegalName         175 //Built-in function names cannot be used for custom function names
#define RES_RecursiveDif        176 //Cannot differentiate recursve functions
#define RES_NoFunc              178 //No function defined
#define RES_NotDifAble          179 //Function cannot be differentiated
#define RES_ErrorType           185 //The function passed as parameter is not correct type
#define RES_CalcError           186 //Not specified error under calculation; Overflow, division by zero, etc.
#define RES_BadGuess            187 //Bad guess for finding trend line
#define RES_NoResult            188 //There was no valid result, eg. from a trend line
#define RES_NoAccurateResult    189 //A result with the required accuracy could not be found
#define RES_InternalError       199 //Unknown error

#define RES_EmptyName           200
#define RES_InvalidName         201
#define RES_EmptyArg            202
#define RES_InvalidArg          203
#define RES_EndParMissing       204
#define RES_CharAfterEndPar     205
#define RES_DoubleDefinedSymbol 206

#define RES_INVALID_RELATION    210

#define RES_UPDATE              300
#define RES_SAVE_CHANGES        301
#define RES_SAVE_CHANGES_IN     302
#define RES_MAX_ZOOM            303
#define RES_MIN_ZOOM            304
#define RES_NOT_EQUAL_ZERO      305
#define RES_ERROR_IN_VALUE      306
#define RES_LIMIT_ZERO          307
#define RES_GREATER_ZERO        308
#define RES_INT_GREATER_ZERO    309
#define RES_ERROR_ZOOM          310
#define RES_GREATER             311
#define RES_MAX_GREATER_MIN     312
#define RES_VALUE_NEEDED        313
#define RES_INVALID_VERSION     314
#define RES_NOT_VALID_NUMBER    315
#define RES_OPEN_FILE           316
#define RES_POSITIVE            317
#define RES_FILE_NOT_FOUND      318
#define RES_NOT_GRAPH_FILE      319
#define RES_ERROR_READING_FILE  320
#define RES_FILE_FILTER         321
#define RES_FILE_ACCESS         322
#define RES_UNKNOWN_FONT        324
#define RES_OLE_GRAPH_SYSTEM    325
#define RES_OLE_OPEN            326
#define RES_SELECTION_ERROR     327
#define RES_FILE_ERROR          328
#define RES_WRITE_FAILED        329
#define RES_OLE_ERROR           330
#define RES_READING_OBJECT      331
#define RES_INVALID_OBJECT      332
#define RES_OBJECT_ERROR        333
#define RES_OUT_OF_RESOURCES    334
#define RES_VALUE_RANGE         335
#define RES_FILE_DOESNT_EXIST   336
#define RES_FILE_READ_ERROR     337
#define RES_PRINTER_ERROR       338
#define RES_INVALID_USE_OF_INF  339
#define RES_OUT_OF_MEMORY       340

#define RES_CAPTION_DELETE      400
#define RES_SHOW                401

#define RES_SAVE_OPTIONS        403
#define RES_MOVING_AVERAGE      404
#define RES_DELETE              405
#define RES_GRAPH_FILTER        406
#define RES_DATA_FILTER         407
#define RES_DELETE_MODEL        408
#define RES_EXPORT_DATA_FILTER  409

#define RES_FIRST_TIP           600
#define RES_LAST_TIP            650
#endif
