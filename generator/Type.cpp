#include "Type.h"

const Type *Type::base_types[MAX_BASE_TYPES];

Type *Type::void_type = NULL;

/* List of all types used *in the program* 
 * We save encountered types to avoid reconstruction */
static vector<Type*> AllTypes;
static vector<Type*> DerivedTypes;

/* Constructor for base types */
Type::Type(eBaseType base_type) :
  eType(eBase),
  ptr_type(0),
  base_type(base_type),
  sid(0),
  used(false)
{
  // Nothing else to do
}

/* Constructor for struct or union types */
Type::Type(vector<const Type*>& struct_fields, bool isStruct, vector<int>& fields_length) :
  ptr_type(0),
  base_type(MAX_BASE_TYPES), // not a valid base type
  fields(struct_fields),
  used(false),
  bitfields_length_(fields_length)
{
  static unsigned int sequence = 0;
  if (isStruct)
    eType = eStruct;
  else
    eType = eUnion;
  sid = sequence++;
}

/* Constructor for pointers */
Type::Type(const Type* t) :
  eType(ePointer),
  ptr_type(t),
  base_type(MAX_BASE_TYPES), // not a valid base type
  used(false)
{
  // Nothing to do
}

Type::~Type(void)
{
  // Nothing to do
}


/* Get base type from enum value */
const Type &
Type::get_base_type(eBaseType st)
{
  static bool initialised = false;

  assert (st != MAX_BASE_TYPES);

  if (!initialised) 
  {
    for (size_t i = 0; i < MAX_BASE_TYPES; i++) 
    {
      Type::base_types[i] = 0;
    }
    initialised = true;
  }

  if (Type::base_types[st] == 0) 
  {
    for (size_t i = 0; i < AllTypes.size(); i++)
    {
      Type *tt = AllTypes[i];
      if (tt->eType = eBaseType && tt->base_type == st)
      {
        Type::base_types[st] = tt;
      }
    }
    if (Type::base_types[st] == 0)
    {
      Type *t = new Type(st);
      Type::base_types[st] = t;
      AllTypes.push_back(t);
    }

  }
  return *Type::base_types[st];
}

/* Get commonly used 'int' type */
const Type *
get_int_type()
{
  return &Type::get_base_type(eInt);
}

/* Find types, return 0 if not found */
Type *
Type::find_type(const Type* t)
{
  for (size_t i = 0; i < AllTypes.size(); i++)
  {
    if (AllTypes[i] == t)
      return AllTypes[i];
  }
  return 0;
}

/* Return 0 if add=false and 't' is not in 'DerivedTypes', hence not pointer */
Type *
Type::find_pointer_type(const Type* t, bool add)
{
  for (size_t i = 0; i < DerivedTypes.size(); i++)
  {
    if (DerivedTypes[i]->ptr_type == t)
      return DerivedTypes[i];
    if (add)
    {
      Type* ptr_type = new Type(t);
      DerivedTypes.push_back(ptr_type);
      return ptr_type;
    }
  }
  return 0;
}

void
Type::make_one_struct_field(vector<const Type*> &fields,
                            vector<int> &fields_length)
{
  fields.push_back(type);
  fields
}






























