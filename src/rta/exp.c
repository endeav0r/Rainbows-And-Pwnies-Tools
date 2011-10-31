#include "exp.h"

struct _rta_exp * rta_exp_create_arith (int type, int bits,
                                        struct _rta_exp * left,
                                        struct _rta_exp * right)
{
    struct _rta_exp * exp;

    exp = (struct _rta_exp *) malloc(sizeof(struct _rta_exp));
    exp->type = type;
    exp->bits = bits;
    exp->value = 0;
    exp->left = left;
    exp->right = right;

    return exp;
}



struct _rta_exp * rta_exp_create (int type, uint64_t value, int bits) {
    struct _rta_exp * exp;

    exp = (struct _rta_exp *) malloc(sizeof(struct _rta_exp));
    exp->type = type;
    exp->value = value;
    exp->bits = bits;
    exp->left = NULL;
    exp->right = NULL;

    return exp;
}


void rta_exp_destroy (struct _rta_exp * exp)
{
    if (exp->left != NULL)
        rta_exp_destroy(exp->left);
    if (exp->right != NULL)
        rta_exp_destroy(exp->right);
    free(exp);
}


uint64_t rta_exp_eval (struct _rta_exp * exp)
{
    uint64_t result = 0;
    
    switch (exp->type) {
    case RTA_EXP_VAR :
        // need to write var code
        break;
    case RTA_EXP_SCONST  :
    case RTA_EXP_UCONST :
        result = exp->value;
        break;
        
    case RTA_EXP_ADD :
    	result = rta_exp_eval(exp->left) + rta_exp_eval(exp->right);
    	break;
    case RTA_EXP_SUB :
    	result = rta_exp_eval(exp->left) - rta_exp_eval(exp->right);
    	break;
    case RTA_EXP_MUL :
    	result = rta_exp_eval(exp->left) * rta_exp_eval(exp->right);
    	break;
    case RTA_EXP_DIV :
    	result = rta_exp_eval(exp->left) / rta_exp_eval(exp->right);
    	break;
    case RTA_EXP_MOD :
    	result = rta_exp_eval(exp->left) % rta_exp_eval(exp->right);
    	break;
        
    case RTA_EXP_ROTL :
    	result = rta_exp_eval(exp->left) << rta_exp_eval(exp->right);
    	break;
    case RTA_EXP_ROTR :
    	result = rta_exp_eval(exp->left) >> rta_exp_eval(exp->right);
    	break;
    

    case RTA_EXP_AND :
    	result = rta_exp_eval(exp->left) & rta_exp_eval(exp->right);
    	break;
    case RTA_EXP_OR :
    	result = rta_exp_eval(exp->left) | rta_exp_eval(exp->right);
    	break;
    case RTA_EXP_XOR :
    	result = rta_exp_eval(exp->left) ^ rta_exp_eval(exp->right);
    	break;
    
    case RTA_EXP_EQ :
    	result = 0;
    	if (rta_exp_eval(exp->left) == rta_exp_eval(exp->right))
    		result = 1;
    	break;
    case RTA_EXP_NE :
    	result = 0;
    	if (rta_exp_eval(exp->left) != rta_exp_eval(exp->right))
    		result = 1;
    	break;
    case RTA_EXP_LT :
    	result = 0;
    	if (rta_exp_eval(exp->left) < rta_exp_eval(exp->right))
    		result = 1;
    	break;
    case RTA_EXP_LTE :
    	result = 0;
    	if (rta_exp_eval(exp->left) <= rta_exp_eval(exp->right))
    		result = 1;
    	break;
    case RTA_EXP_GT :
        result = 0;
        if (rta_exp_eval(exp->left) > rta_exp_eval(exp->right))
            result = 1;
        break;
    case RTA_EXP_GTE :
        result = 0;
        if (rta_exp_eval(exp->left) >= rta_exp_eval(exp->right))
            result = 1;
        break;
    }
    
    switch (exp->bits) {
    	case 1  : return result & 0x1;
    	case 8  : return result & 0xff;
    	case 16 : return result & 0xffff;
    	case 32 : return result & 0xffffffff;
    }
    return result;
}
