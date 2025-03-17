#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h> // for atoi
#include <limits.h>  

enum {
  TK_NOTYPE = 256, TK_EQ,


  /* TODO: Add more token types */
 TK_NUM, TK_DEREF,TK_REG,TK_HEX
};
  
static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\$[a-z]+", TK_REG},  // 寄存器表达式 
  {"0x[0-9a-fA-F]+", TK_HEX}, 
  {"\\*", TK_DEREF},        // 指针解引用*(必须放在前面)
  {"[0-9]+", TK_NUM},
  {"\\+", '+'},         // plus
  {"-", '-'},
  {"\\*", '*'}, 
  {"/", '/'},
  {"\\(", '('},         
  {"\\)", ')'},
  {"==", TK_EQ},         // equal
  

  
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];// no more than 32
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          //default: TODO();
           case TK_NOTYPE: 
   		 break;

	   case TK_NUM:
	       		case TK_DEREF:
				case TK_HEX:
   		  case TK_REG:
  	   case '+': case '-': case '*': case '/':
 	   case '(': case ')':
           if (nr_token >= 32)
	   {
		panic("Too many tokens");
   	   }
	   tokens[nr_token].type = rules[i].token_type;
           strncpy(tokens[nr_token].str, substr_start, substr_len);
           tokens[nr_token].str[substr_len] = '\0';
           nr_token++;
                break;

	   default:
    		panic("Unexpected token");
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p, int q, bool *success) {
  if (tokens[p].type != '(' || tokens[q].type != ')') return false;
  int balance = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') balance++;
    else if (tokens[i].type == ')') balance--;
    if (balance < 0) { *success = false; return false; }
  }
  return (balance == 0);
}//简单小算法题

static uint32_t eval(int p,int q,bool *success)//改了一点模板
{
  if (p > q || !*success) { *success = false; return 0; }
  if (p == q) {
      switch (tokens[p].type) {
      case TK_NUM: return atoi(tokens[p].str);
      case TK_HEX: return strtol(tokens[p].str, NULL, 16);
      default:
    *success = false; return 0;
  }
  }
  if (check_parentheses(p, q, success)) return eval(p+1, q-1, success);
  
  int op_pos = -1, min_prio = INT_MAX, balance = 0;

  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') balance++;
    else if (tokens[i].type == ')') balance--;
    if (balance != 0) continue;

	int current_prio = -1;
	switch (tokens[i].type) {
		case '+': case '-': current_prio = 1; break;
		case '*': case '/': current_prio = 2; break;
		default: continue;
	}

	if (current_prio <= min_prio) { // 正确逻辑
		min_prio = current_prio;
		op_pos = i;
		}
	}

  if (op_pos == -1) { *success = false; return 0; }
 
  uint32_t val1 = eval(p, op_pos-1, success);
  uint32_t val2 = eval(op_pos+1, q, success);

  switch (tokens[op_pos].type) {
    case '+': return val1 + val2;
    case '-': return val1 - val2;
    case '*': return val1 * val2;
    case '/': 
      if (val2 == 0) { *success = false; return 0; }//不能除以0
      return val1 / val2;
    default: 
      *success = false; return 0;
  }
 return 0; 
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  *success = true;
  return eval(0, nr_token-1, success);
}
