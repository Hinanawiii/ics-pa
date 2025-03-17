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
  {"\\$[a-zA-Z]+", TK_REG},  // 寄存器表达式 
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
//辅助函数单目负号判断
static bool is_unary_minus(int pos) {
    if (tokens[pos].type != '-') return false;
    // 判断是否是表达式开头或前一个token为运算符/左括号
    if (pos == 0) return true;
    int prev_type = tokens[pos - 1].type;
    return (prev_type == '(' || prev_type == '+' || prev_type == '-' || 
            prev_type == '*' || prev_type == '/');
}

static uint32_t eval(int p,int q,bool *success)//改了一点模板
{
  if (p > q || !*success) { *success = false; return 0; }
  if (is_unary_minus(p)) {
     uint32_t val = eval(p+1, q, success);
     return (uint32_t)(-(int32_t)val);  // 注意处理补码转换
  }
  if (tokens[p].type == TK_DEREF) {
    if (p + 1 > q) { *success = false; return 0; } 
    uint32_t addr = eval(p + 1, q, success);
    return vaddr_read(addr, 4);
  }
  if (p == q) {
      switch (tokens[p].type) {
      case TK_NUM: return atoi(tokens[p].str);
      case TK_HEX: return strtol(tokens[p].str, NULL, 16);
			case TK_REG: {
    const char *reg_name = tokens[p].str + 1; // 去掉 '$'
    bool found = false;
    uint32_t value = 0;
    
    // 遍历所有可能的寄存器名称列表
    for (int i = 0; i < 8; i++) {
        // 检查32位寄存器（eax, ecx...）
        if (strcmp(reg_name, regsl[i]) == 0) {
            value = reg_l(i);
            found = true;
            break;
        }
        // 检查16位寄存器（ax, cx...）
        if (strcmp(reg_name, regsw[i]) == 0) {
            value = reg_w(i);
            found = true;
            break;
        }
        // 检查8位寄存器（al, ah...）
        if (i < 4) {
            if (strcmp(reg_name, regsb[i]) == 0) { // 低8位（al, cl...）
                value = reg_b(i);
                found = true;
                break;
            }
        } else {
            if (strcmp(reg_name, regsb[i]) == 0) { // 高8位（ah, ch...）
                value = reg_b(i);
                found = true;
                break;
            }
        }
    }
    
    if (!found) {
        *success = false;
        printf("Unknown register: %s\n", reg_name);
    }
    return value;
}
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
	bool is_unary = false;//需要这个变量归正运算
	     if (is_unary_minus(i)) {
            current_prio = 4;  // 最高优先级
            is_unary = true;
        } else{
	switch (tokens[i].type) {
		case '+': case '-': current_prio = 1; break;
		case '*': case '/': current_prio = 2; break;
		case TK_DEREF: current_prio = 3; break; //优先级最高
		default: continue;
	}
	}

    if (is_unary) {
        // 单目运算符直接选择最左侧的负号
        op_pos = i;
        min_prio = current_prio;
        break; // 单目负号优先级最高，无需继续查找
    } else if (current_prio <= min_prio) {
        min_prio = current_prio;
        op_pos = i;
    }
	}

  if (op_pos == -1) { *success = false; return 0; }
 
  uint32_t val1 = eval(p, op_pos-1, success);
  uint32_t val2 = eval(op_pos+1, q, success);
  
  if (is_unary_minus(op_pos)) {
      uint32_t val = eval(op_pos + 1, q, success);
      return (uint32_t)(-(int32_t)val);
  }

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
