/* Types are defined inductively:
 *   
 *   Type ::= (BaseType) | (`Pointer` Type) | (`Struct` [Type]) | 
 *            (`Union` [Type])
 *
 * So for every 'Type' we can use DFS to get some base 'SimpleType'.
 * The other information about how to construct the full type will
 * then be available based upon whether it is a struct, union or pointer,
 * and the path we have taken to get to the base 'SimpleType'.
 *
 * */


#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <ostream>
#include <vector>

enum eTypeDesc
{
  eBase,
  ePointer,
  eUnion,
  eStruct
};

// The values inside enums are always of integral type
#define MAX_TYPE_DESC ((eTypeDesc) (eStruct+1))

enum eBaseType
{
  eVoid,
  eChar,
  eInt,
  eShort,
  eLong,
  eLongLong,
  eUChar,
  eUInt,
  eUShort,
  eULong,
  // eFloat,
  // eDouble,
  eULongLong
};

#define MAX_BASE_TYPES ((eTypeDesc) (eULongLong+1))

class Type
{
  public:
    static const Type *choose_base_type(void);
    static const Type *choose_non_void_type();
    static const Type *choose_pointer_type(void);
    static const Type *choose_struct_from_type(const Type* type);
    static bool has_pointer_type(void);
    static void copy_all_fields_from_types(vector<const Type*> &dest_types, vector<const Type*> &src_types);
    
  private:
    eTypeDesc eType;
    const Type *ptr_type;
    eBaseType simple_type;
    vector<unsigned int> dimensions;  // For arrays
    vector<const Type*> fields;       // For structs/unions
    unsigned int sid;                 // Sequence id for structs/unions
    bool used;                        // Whether any variable declared with this type
    vector<int> bitfields_length_;    // For struct/unions. '-1' means regular field
    static Type *void_type;
    static const Type *base_types[MAX_BASE_TYPES];
};

#endif
