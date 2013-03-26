#!/usr/bin/python2

from ply import *


class Compiler:
	# some constrait
	MAX_INT = 2**31-1
	VALID_CHAR = ''.join( [ chr(x) for x in range(32,127) ] )
	MAX_STRING_LENGTH = 255
	MAX_IDENTIFIER_LENGTH = 255

	# tokens here
	myLiterals = ['IDENTIFIER','INTEGERT','REALT','STRINGT']
	myKeywords = ['PROGRAM','IS','BEGIN', 'END', 
	'VAR', 'TYPE', 'PROCEDURE', 'ARRAY', 'RECORD','IN', 
	'OUT', 'READ', 'WRITE', 'IF','THEN', 'ELSE', 
	'ELSIF', 'WHILE', 'DO','LOOP', 'FOR', 'EXIT', 
	'RETURN', 'TO', 'BY', 'AND', 'OR', 'NOT', 'OF', 
	'DIV', 'MOD']
	myOperators = {'LPAREN':'(','RPAREN':')',
	'LBRACKET':'[','RBRACKET':']','LBRACE':'{','RBRACE':'}',
	'COLON':':','DOT':'.','SEMICOLON':';','COMMA':',',
	'ASSIGN':':=','PLUS':'+','MINUS':'-','STAR':'*',
	'SLASH':'/','BACKSLASH':'\\','EQ':'=',
	'NEQ':'<>','LT':'<','LE':'<=','GT':'>',
	'GE':'>=','LABRACKET':'[<','RABRACKET':'>]'}

	tokens = myLiterals + myKeywords + myOperators.keys()

	# ---------------------------
	#    lexer
	# ---------------------------

	# utilities...
	def find_column(self,input,token):
		last = input.rfind('\n',0,token.lexpos)
		if last < 0:
			last = 0
		column = ( token.lexpos - last ) + 1
		return column

	def represent(self,token):
		if token.type in self.myLiterals:
			s = token.type + '(' + str(token.value) + ')'
			return s
		elif token.type in self.myKeywords:
			return token.type
		elif token.type in self.myOperators:
			return self.myOperators[ token.type ]

	# re(regular expression)
	# in plx, the order of re is
	# 1. defined by funcions (def t_ID), in their order from file
	# 2. defined by string, in reverse order of re length

	def t_IDENTIFIER(self,t):
		r'[a-zA-Z_][a-zA-Z_0-9]*'
		t.type = t.value if t.value in self.myKeywords else 'IDENTIFIER'
		if t.type == 'IDENTIFIER':
			if len(t.value)>self.MAX_IDENTIFIER_LENGTH:
				t.value = t.value[:self.MAX_IDENTIFIER_LENGTH]
				self.lex_error( "Identifier too long, striped.", t )

		return t

	# including 1.1 (in language spefification) 
	# or 1. or .1 (my idea, like in C)
	def t_REALT(self,t):
		r'[0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+'
		t.value = float(t.value)
		return t

	def t_INTEGERT(self,t):
		r'[0-9][0-9]*'
		t.value = int(t.value)
		if t.value > self.MAX_INT:
			self.lex_error( "Integer out of range, changed to %d"%(self.MAX_INT) , t)
		return t

	def t_STRINGT(self,t):
		r'\".*?(?:\"|\n|\r)'

		ori_value = t.value
		# handle unterminated
		if ori_value[-1] != '"':
			self.lex_error( "Unterminated string, forced closed",t )

		t.value = t.value[1:-1]
		# remove '"' at begining, '"' or '\n' or '\r' at ending

		# handle illegal chars
		s = ''.join([c for c in list(t.value) if c in self.VALID_CHAR])
		if s != t.value:
			icl = ','.join(['\\'+hex(ord(c)) for c in t.value if c not in self.VALID_CHAR]);
			self.lex_error("Illegal characters ingored. there are "+icl,t)
		if len( t.value ) > self.MAX_STRING_LENGTH:
			self.lex_error( "String too long, striped." ,t)
			t.value = t.value[:self.MAX_STRING_LENGTH]
		return t

	t_LPAREN = r'\('
	t_RPAREN = r'\)'
	t_LBRACKET = r'\['
	t_RBRACKET = r'\]'
	t_LBRACE = r'\{'
	t_RBRACE = r'\}'
	t_COLON = r'\:'
	t_DOT = r'\.'
	t_SEMICOLON = r'\;'
	t_COMMA = r'\,'
	t_ASSIGN = r'\:\='
	t_PLUS = r'\+'
	t_MINUS = r'\-'
	t_STAR = r'\*'
	t_SLASH = r'\/'
	t_BACKSLASH = r'\\'
	t_EQ = r'\='
	t_NEQ = r'\<\>'
	t_LT = r'\<'
	t_LE = r'\<\='
	t_GT = r'\>'
	t_GE = r'\>\='
	t_LABRACKET = r'\[\<'
	t_RABRACKET = r'\>\]'

	#specail
	def t_newline(self,t):
		r'\n+'
		t.lexer.lineno += len(t.value)

	t_ignore = ' \t'

	def t_COMMENT(self,t):
		r'\(\*(?:.|\n|\r)*?\*\)'
		pass

	def t_OPEN_COMMENT(self,t):
		r'\(\*(?:.|\n|\r)*'
		self.lex_error( "Unterminated comment, forced close." ,t )
		pass

	def t_error(self,t):
		self.lex_error("Illegal char \\%s" % hex(ord(t.value[0])) , t)
		t.lexer.skip(1)


	def lex_error(self,info,token):
		line_no = token.lexer.lineno
		column_no = self.find_column( self.data, token )
		print "Token Error : Line %2d, Column %2d : %s" %(line_no,column_no,info)


	def run_lex(self,data):
		self.lex.input(data)
		self.data = data
		while True:
			token = self.lex.token()
			if not token:
				break

			line_no = self.lex.lineno
			column_no = self.find_column( data, token )

			output_line = ""
			output_line += "Token : Line %2d, Column %2d : " %(line_no,column_no)
			output_line += self.represent( token )
			print output_line


	# --------------------
	#     Parser
	# --------------------

	# Abstrct Syntax Tree
	# terminal node like 'ARRAY'
	# non-terminal node like 'FieldDecl'

	class AST:
		def __init__(self):
			pass

	def __init__(self):
		self.lex = lex.lex(module=self)




# actual working


import sys
filename = sys.argv[1]
data=open(filename,'r').read()


compiler = Compiler()
compiler.run_lex(data)
