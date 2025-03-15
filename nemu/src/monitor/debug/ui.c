#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
static int cmd_si(char *args) ;
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },


  /* TODO: Add more commands */
  {"si", "Step through N instructions", cmd_si},
  { "info", "Print program status", cmd_info },
  { "x", "Scan memory", cmd_x },
  {"p", "Evaluate expression", cmd_p},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){

  char *arg = strtok(NULL, " ");
  int steps = 1;  // default value
  
  if (arg != NULL) {
    steps = atoi(arg);
    if (steps <= 0) {
      printf("Invalid step count: %s\n", args);
      printf("Invalid number of steps. Using default (1).\n");
      steps = 1;
    }
  }
  
  printf("Executing %d step(s)...\n", steps);
  cpu_exec(steps);
  return 0;

}

static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("Try 'r' for registers or 'w' for watchpoints.\n");
    return 0;
  }
  
  if (strcmp(arg, "r") == 0) {
        // 打印完整寄存器视图
        printf("-----------------------------------------\n");
        printf("| %-4s | %-12s | %-4s | %-12s |\n", 
            "32bit", "Value", "8bit", "Value");
        
        for (int i = 0; i < 8; i++) {
            // 打印32位寄存器及其对应的16/8位寄存器
            printf("|------|-------------|------|-------------|\n");
            printf("| %-4s | 0x%08x  | %-4s | 0x%08x  |\n", 
                regsl[i], reg_l(i),
                regsb[i], reg_b(i));
            
            // 单独处理高位寄存器(AH,CH,DH,BH)
            if (i < 4) { // 只有前4个寄存器有高位
                printf("|      |             | %-4s | 0x%08x  |\n",
                    regsb[i+4], (reg_l(i) >> 8) & 0xff);
            }
            
            // 打印16位寄存器视图
            printf("| %-4s | 0x%08x  |      |             |\n",
                regsw[i], reg_w(i));
        }
        printf("-----------------------------------------\n");
        
        // 打印EIP
        printf("eip: 0x%08x\n", cpu.eip);
        return 0;
    }
  else if (strcmp(arg, "w") == 0) {
    // Print watchpoint information
    printf("Watchpoints not implemented.\n");
  } 
  else {
    printf("Unknown info subcommand '%s'\n,retry", arg);
  }
  
  return 0;
}

static int cmd_x(char *args) {
    char *arg1 = strtok(args, " ");
    char *arg2 = strtok(NULL, " ");
    
    if (arg1 == NULL || arg2 == NULL) {
        printf("Usage: x N 0xADDR\n");
        return 0;
    }

    int count = atoi(arg1);
    if (count <= 0) {
        printf("Invalid count: %s\n", arg1);
        return 0;
    }

    // 只支持十六进制数字
    uint32_t addr;
    if (sscanf(arg2, "0x%x", &addr) != 1) {
        printf("Invalid address format: %s\n", arg2);
        return 0;
    }

    printf("Address    : Value\n");
    printf("------------------\n");
    for (int i = 0; i < count; i++) {
        uint32_t value = vaddr_read(addr + i*4, 4); 
        printf("0x%08x: 0x%08x\n", addr + i*4, value);
    }
    return 0;
}

static int cmd_p(char *args) {
    if (args == NULL) {
        printf("Usage: p EXPR\n");
        return 0;
    }
    
    bool success;
    uint32_t result = expr(args, &success);
    
    if (success) {
        printf("Result: %u (0x%08x)\n", result, result);
    } else {
        printf("Invalid expression: %s\n", args);
    }
    return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
