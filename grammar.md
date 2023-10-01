# Pietra AST Parsing Rules
## packages    
    use std::String;
    use std::*;
    use std1::std2:: ... ::stdfinal;

    package_base    = NAME | "{" NAME_LIST "}"
    package         = package_base ("::" package_base)*+ ("::*")*
    
    

## types
    &       = address
    mut     = mutable
    
    
    typespec_base
        = ('&' ('mut')* | '*')* NAME
        | 'proc' '(' type_list? ')' (':' typespec)?
        | '(' typespec ')'

    typespec
        = typespec_base ("<" typespec ">")
        | "proc" "(" typespec_list ")" ":" typespec 

## expressions
    [NOTE]: `new` keyword can accepts base_expr with kind of (NAME | COMPOUND)

    compound_expr   =   "[" base_expr "]" = expr | ":" base_expr = expr | expr    
    literal_expr    =   INT | FLOAT | STRING | NAME |  (:type){compound_expr} | {} | new ([base_expr])* base_expr
    base_expr       =   literal_expr ("::" base_expr | "(" expr_list ")" | "[" expr "]" | "." base_expr)*+
    unary_expr      =   (UNARY) base_expr
    mult_expr       =   unary_expr (MUL unary_expr)*+
    add_expr        =   mult_expr (ADD mult_expr)*+
    cmp_expr        =   add_expr (CMP add_expr)*+
    logic_expr      =   cmp_expr (LOGIC cmp_expr)*+
    cast_expr       =   logic_expr ("as" typespec)*
    ternary_expr    =   cast_expr ("if" cast_expr "else" cast_expr)*    

## statements
    for_stmt
        = for x:i64 in expr // loop x in expr.begin till expr.end
          for expr // declare x as it
          for {} // infinite loop
    stmt 
        = "if" 
        | "switch"
        | "for" for_stmt
        | "while"
        | expr  
        
## declarations
    proc_proto
        = "(" (proc_params = NAME: typespec)*+ ")"
    decl_base
        = (proc)* proc_proto (":" typespec) stmt_block
        | (struct|union)* { aggregate_item*+ }
        | enum { enum_items*+ }
        | type CString = *char;
        

    decl
        = NAME "::" decl_base


