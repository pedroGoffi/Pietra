" Vim syntax file
" Language: Porth

" Usage Instructions
" Put this file in .vim/syntax/porth.vim
" and add in your .vimrc file the next line:
" autocmd BufRead,BufNewFile *.porth set filetype=porth

if exists("b:current_syntax")
  finish
endif

set iskeyword=a-z,A-Z,-,*,_,!,@
syntax keyword porthTodos TODO XXX FIXME NOTE

" Language keywords
syntax keyword porthKeywords if elif else while true false and or struct enum union 
" Comments
syntax region porthCommentLine start="//" end="$"   contains=porthTodos

" String literals
syntax region porthString start=/\v"/ skip=/\v\\./ end=/\v"/ contains=porthEscapes

" Char literals
syntax region porthChar start=/\v'/ skip=/\v\\./ end=/\v'/ contains=porthEscapes

" Escape literals \n, \r, ....
syntax match porthEscapes display contained "\\[nr\"']"

" Number literals
syntax region porthNumber start=/\s\d/ skip=/\d/ end=/\s/

" Type names the compiler recognizes
syntax keyword porthTypeNames i8 i16 i32 i64 f32 f64 bool null
" Set highlights
highlight default link porthTodos Todo
highlight default link porthKeywords Keyword
highlight default link porthCommentLine Comment
highlight default link porthString String
highlight default link porthNumber Number
highlight default link porthTypeNames Type
highlight default link porthChar Character
highlight default link porthEscapes SpecialChar

let b:current_syntax = "pietra"
