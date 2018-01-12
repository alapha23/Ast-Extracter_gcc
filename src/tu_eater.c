#include "tu_eater.h" 


int main(int argc, char **argv)
{
	size_t filesize;

	open_file(argv);
	filesize = get_filesize();
	
	pool = (node **)calloc(filesize/70, sizeof(node*));
	eval_file(argv[2]);
	free(pool);
}


/*node* search_pool(int id, node **pool, int n)
{
                int i;
                for(i=0; i<n; i++)
                {
                        if(id == (get_by_num(i, pool, n)->_id))
                                break;
			node *temp = get_by_num(i, pool, n);
                }
                return get_by_num(i, pool, n);
}*/ 
/*
 *
 *	temp = search_pool(1845, pool, n_inpool);
	DEBUF("%p", temp->to_dot);
	DEBUF("%s", temp->_inner);
	assert(temp->to_dot != NULL);
	temp->to_dot(temp);
	dump_node(temp);

 *
 * **/


static node * 
eval_node(void)
{
	// some node occupy more than 1 line
	char line[72];
	node *n = (node *)calloc(1, sizeof(node));
	char next;
	int inner_len = 0;

	n->_inner = (char *)calloc(INNER_SIZE, 1);

	// read a new line
	if(fgets(line, 72, fp) == NULL)
	       return NULL;	
	// set type, to_do, _inner and id 
	eval_ntype(line, n);

	while(((next = peek()) != EOF)&&(next != '@'))
	// next line belongs to this node unless EOF or @
	{
		memset(line, 0, 72);
		fgets(line, 72, fp);
		strcat(n->_inner, line);
		inner_len++;
		if(inner_len*72 > 2000)
		{
			DEBUG(Inner content exceed default);
			DEBUG(Please modify INNER_SIZE in the header file);
			exit(0);
		}
	}
	ADD2POOL(n, pool, n_inpool);
	if(peek() == EOF)
		return NULL;

	return n;
}

static int *read_op_inner(char *inner, int *num_op)
{
	char ch = ':';
	// we suppose the form is type: @x op 0: @xx op 1: @xx
	int *op = calloc(NUM_EXPR, sizeof(int));

	assert(NULL != num_op);
	// the first one must be a type
	if(strchr(inner, ch) == NULL)
		return NULL;
	else
		inner = strchr(inner, ch) + 1;

	do
	{
		int op_id;
		inner = strchr(inner, ch) + 1;

		(*num_op) = *(inner-2) - 48 + 1;
		sscanf(inner, " @%d ", &op_id);
		*(op+(*num_op)-1) = op_id;
	}while(strchr(inner, ':') != strrchr(inner, ':'));
	return op;
}

static void minus_to_dot(node *n)
{
// minus_expr is only called when two poiner operators do minus
// lefthand = op0 - op1
	int op0_id, op1_id;
	node *op0;
	node *op1;

	sscanf(n->_inner, " %*s %*s op 0: @%d op 1: @%d", &op0_id, &op1_id);	
	op0 = search_pool(op0_id, pool, n_inpool);
	op1 = search_pool(op1_id, pool, n_inpool);

	assert(NULL != op0);
	assert(NULL != op1);

	n->_dot_id = dot_shape(n->_id, "-");
	op0->prev = n;
	op1->prev = n;

	op0->to_dot(op0);
	op1->to_dot(op1);

	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void component_to_dot(node *n)
{
	// called when function(a->b.c); occurs
	// component_ref    type: @9141    op 0: @12854   op 1: @9667
	//
	int op_id, field_id;
	node *op;	// op0
	node *field;	// op1

	sscanf(n->_inner, " %*s %*s op 0: @%d op 1: @%d", &op_id, &field_id);
	op = search_pool(op_id, pool, n_inpool);	// might be component
	field = search_pool(field_id, pool, n_inpool);
	assert(NULL != op);
	assert(NULL != field);

	// DEBUG
	n->_dot_id = dot_shape(ref->_id, "component");
	dot_link(n->prev->_dot_id, n->_dot_id);

	op->prev = n;
	op->to_dot(op);
	// n:  "*"
/*
	if(op->_ntype == component_ref)
	{
	n->_dot_id = dot_shape(ref->_id, "*");
	dot_link(n->prev->_dot_id, n->_dot_id);

		op->to_dot(op);
	}
	else if(op->_ntype == indirect_ref)
	{
	n->_dot_id = dot_shape(ref->_id, "*");
	dot_link(n->prev->_dot_id, n->_dot_id);

		int parm_id;
		node *parm;
		sscanf(op->_inner, " %*s %*s op 0: @%d ", &parm_id);
		parm = search_pool(parm_id, pool, n_inpool);
		parm->prev = op->prev;
		parm->to_dot(parm);
	}
	else if(op->_ntype == var_decl)
	{
		op->prev = n->prev;
		op->to_dot(op);
		n->_dot_id = op->_dot_id;
		n = op;
	}else
	{
		DEBUF("Unknown component op type: %p", op->_ntype);
	}
*/
	field->prev = n;
	field->_dot_id = dot_shape(field->_id, ".");
	dot_link(field->prev->_dot_id, field->_dot_id);
	int name_id;
	node *name;
	char value[64];
	sscanf(field->_inner, " name: @%d ", &name_id);
	name = search_pool(name_id, pool, n_inpool);
	name->prev = field;		// name connect to field "."
	sscanf(name->_inner, " strg: %s ", value);
	name->_dot_id = dot_shape(name->_id, value);
	dot_link(name->prev->_dot_id, name->_dot_id);
}

static void lshift_to_dot(node *n)
{
	int op0_id, op1_id;
	node *op0;
	node *op1;

	sscanf(n->_inner, " %*s %*s op 0: @%d op 1: @%d", &op0_id, &op1_id);	
	op0 = search_pool(op0_id, pool, n_inpool);
	op1 = search_pool(op1_id, pool, n_inpool);

	assert(NULL != op0);
	assert(NULL != op1);

	n->_dot_id = dot_shape(n->_id, "<<");
	op0->prev = n;
	op1->prev = n;

	op0->to_dot(op0);
	op1->to_dot(op1);

	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void rshift_to_dot(node *n)
{
	int op0_id, op1_id;
	node *op0;
	node *op1;

	sscanf(n->_inner, " %*s %*s op 0: @%d op 1: @%d", &op0_id, &op1_id);	
	op0 = search_pool(op0_id, pool, n_inpool);
	op1 = search_pool(op1_id, pool, n_inpool);

	assert(NULL != op0);
	assert(NULL != op1);

	n->_dot_id = dot_shape(n->_id, ">>");
	op0->prev = n;
	op1->prev = n;

	op0->to_dot(op0);
	op1->to_dot(op1);

	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void negate_to_dot(node *n)
{
	int op_id;
	node *op;
	sscanf(n->_inner, " %*s %*s op 0: @%d", &op_id);

	op = search_pool(op_id, pool, n_inpool);
	assert(NULL != op);

	n->_dot_id = dot_shape(n->_id, "-");
	op->prev = n;
	op->to_dot(op);

	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void sub_stmt_to_dot(node *n)
{
	node **node_list;
	int num_stmt;

 	char *inner = n->_inner;
	int pos = strlen(inner)-1;
	while(pos)
	{
		if(*(inner+pos) == ':')
		{
			// the last node
			pos--;
			while(1)
			{
				if(*(inner+pos) != ' ')
					break;
				pos--;
			}
			while(1)
			{
				if(*(inner+pos) != ' ')
				{
					pos--;
				}else
					break;
			}
			break;
		}
		pos--;
	}
	pos++;
        sscanf(inner+pos, "%d ", &num_stmt);

	// num_stmt+2
	// prev node is included
	node_list = calloc(num_stmt+2, sizeof(node *));

	*node_list = n->prev;

	// we use prev node
	// as current node is stmt_list
	pos = 3;
	int nstmt = 1;
	while(pos != strlen(inner))
	{
		if(*(inner+pos) == '@')	
		{
			node *temp;
			int temp_id;
			sscanf(inner+pos, "@%d ", &temp_id);
			temp = search_pool(temp_id, pool, n_inpool);

			if(temp->_ntype == decl_expr)
			{
				pos++;
				continue;
			}
			*(node_list+nstmt) = temp;
			// it is vital to connect them with prev ptr
			temp->prev = *(node_list+nstmt-1);
			nstmt++;
		}
		pos++;	
	}

	// now dump all the stmt
	// now pos stands for which node in the list we are dumping
	pos = 1;	
	while(pos != nstmt)
	{

		if((*(node_list+pos))->_ntype!=decl_expr)
			(*(node_list+pos))->to_dot(*(node_list+pos));
		pos++;

	}
}

static void predict_to_dot(node *n)
{
	// predict expr exists in continue expr
	n->_dot_id = n->prev->_dot_id;
}

static void addr_to_dot(node *n)
{
	node *ptr_type;
	node *type;
	int ptr_type_id, type_id;

	// check type of addr
	sscanf(n->_inner, " type: @%d ", &ptr_type_id);
	ptr_type = search_pool(ptr_type_id, pool, n_inpool);
	sscanf(ptr_type->_inner, " %*s %*s %*s %*d ptd : @%d", &type_id);
	type = search_pool(type_id, pool, n_inpool);

	if(type->_ntype == array_type )
	{
		// print if it is needed
		node *op;
		int op_id;
		sscanf(n->_inner, " type: @%*d op 0: @%d ", &op_id);
		op = search_pool(op_id, pool, n_inpool);
		op->prev = n->prev;
		op->to_dot(op);
	}
	else
	{
		n->_dot_id = dot_shape(n->_id, "&");
		dot_link(n->prev->_dot_id, n->_dot_id);
		node *op;
		int op_id;
		sscanf(n->_inner, " type: @%*d op 0: @%d ", &op_id);
		op = search_pool(op_id, pool, n_inpool);
		op->prev = n;
		op->to_dot(op);
	}
}

static void string_cst_to_dot(node *n)
{
	int size;
	int type_id, size_id;
	node *type;
	node *size_n;
	
	// obtain the size of the string
	sscanf(n->_inner, " type: @%d ", &type_id);
	type = search_pool(type_id, pool, n_inpool);

	sscanf(type->_inner, " size: @%d ", &size_id);
	size_n = search_pool(size_id, pool, n_inpool);

	sscanf(size_n->_inner, " type: @%*d %*s %d ", &size);

	char value[size];
	memset(value, 0, size);
	int pos = 0;
	char *inner = n->_inner + 12;
	while(pos != strlen(inner))
	{
		if(*(inner+pos) == ':')
		{
			pos += 2;
			int counter = 0;
			while((*(inner+pos+counter+2) != 'l') && counter <= size)
			{
				value[counter] = *(inner+pos+counter);	
				counter++;
			}
			break;
		}
		pos++;
	}
	char result[size+4];
	sprintf(result, "\\\"%s\\\"", value);
	n->_dot_id = dot_shape(n->_id, result);
	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void nop_to_dot(node *n)
{
	int op_id;
	node *op;

	sscanf(n->_inner, " type: @%*d op 0: @%d", &op_id);
	op = search_pool(op_id, pool, n_inpool);
	op->prev = n->prev;
	op->to_dot(op);
}

static void bind_to_dot(node *n)
{
	int body_id;
	node *body;

	sscanf(n->_inner, " %*[^b] body: @%d", &body_id);
	body = search_pool(body_id, pool, n_inpool);
	body->prev = n->prev;
	body->to_dot(body);

	int counter = strlen(body->_inner)-1;
	int last_node_id;
	node *last_node;
	while(counter != 0)
	{
		if(*(body->_inner+counter) == '@')
		{
			sscanf(body->_inner+counter+1, "%d", &last_node_id);
			break;
		}
		counter--;
	}
	last_node =  search_pool(last_node_id, pool, n_inpool);
	n->_dot_id = last_node->_dot_id;
}

static void eq_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "==");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}


static void ne_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "!=");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}


static void gt_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, ">");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void ge_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, ">=");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void lt_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "<");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void le_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "<=");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void goto_to_dot(node *n)
{
	int index;
	char value[64];

	sscanf(n->_inner, "%*s @%*d labl: @%d ", &index);
	sprintf(value, "goto label%d", index);

	n->_dot_id  = dot_shape(n->_id, value);

	// connect with the previous node
	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void label_to_dot(node *n)
{
	int index;
	char value[32];

	sscanf(n->_inner, "%*s @%*d name: @%d ", &index);
	sprintf(value, "label%d:", index);

	n->_dot_id  = dot_shape(n->_id, value);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void cond_to_dot(node *n)
{
	// if(){}
	// 	stmt
	// cond_expr        type: @151     op 0: @1881    op 1: @1882
	// op 1: statement_list; op 0: expr
	// 
	// if(){}else{}
	// cond_expr	type: @151     op 0: @1881    op 1: @1882
	//                         op 2: @1883
	// op 1& op 2 would be stmt in the if_body and else_body
	// it might be statement_list or a xxx_expr
	// 
	// if(){}else if(){}
	// op 2 would be a cond_expr
	//
	// while(){}, aka:
	// goto labl0;
	// labl1:
	// 	stmt;
	// labl0:
	// if()
	// 	goto labl1;
	// 	goto labl2;
	// labl2:
	// 	stmt
	//
	// cond_expr        type: @151     op 0: @1888    op 1: @1889
	//                         op 2: @1890
	// op 1: goto_expr; op 2: goto_expr
	// 

	int id1, id2, id3;
	int num_op = 0;
	int *op;

	op = read_op_inner(n->_inner, &num_op);

	if(num_op > 2)
	{
		DEBUF("id: %d", n->_id);
		DEBUF("cond_expr: num of op error = %d", num_op);
		DEBUF("inner:%s", n->_inner);
		exit(0);
	}
	else if(num_op == 1)
	{
		// if(){}
		sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

		node *op1 = search_pool(id1, pool, n_inpool);
		node *op2 = search_pool(id2, pool, n_inpool);

		assert(op1 != NULL);
		assert(op2 != NULL);

 		n->_dot_id = dot_shape(n->_id, "if");
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
		op1->prev = n;
		op2->prev = n;
		op1->to_dot(op1);
		// op1 is a vardecl, it should be emiting its name
		op2->to_dot(op2);

		// connect with the previous node
	
	} else
	{
		sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d op 2: @%d ", &id1, &id2, &id3);
		node *op1 = search_pool(id1, pool, n_inpool);
		node *op2 = search_pool(id2, pool, n_inpool);
		node *op3 = search_pool(id3, pool, n_inpool);

		assert(op1 != NULL);
		assert(op2 != NULL);
		assert(op3 != NULL);

		if(op2->_ntype == goto_expr)
		{
		// do(){}while;
		// while(){}
 			n->_dot_id = dot_shape(n->_id, "if");

			// connect with the previous node
			dot_link_dt(n->prev->_dot_id, n->_dot_id);		

			op1->prev = n;
			op2->prev = n;
			op3->prev = n;
			op1->to_dot(op1);
			// op1 is the comparsion
			op2->to_dot(op2);
			op3->to_dot(op3);

		}
		else
		{
			// if(){}else{}
			// if(){}else if{}

 			n->_dot_id = dot_shape(n->_id, "if");

			// connect with the previous node
			dot_link_dt(n->prev->_dot_id, n->_dot_id);

			op1->prev = n;
			op2->prev = n;
			op3->prev = n;
			op1->to_dot(op1);
			// op1 is the comparsion
			op2->to_dot(op2);
			if(op3->_ntype == cond_expr)
				op3->to_dot = else_to_dot;
			op3->to_dot(op3);
		}
	}
	free(op);
}

static void else_to_dot(node *n)
{
	int id1, id2, id3;
	int num_op = 0;
	int *op;

	op = read_op_inner(n->_inner, &num_op);

	if(num_op > 2)
	{
		DEBUF("id: %d", n->_id);
		DEBUF("cond_expr, else assumed: num of op error = %d", num_op);
		DEBUF("inner:%s", n->_inner);
		exit(0);
	}
	else if(num_op == 1)
	{
		// if(){}
		sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

		node *op1 = search_pool(id1, pool, n_inpool);
		node *op2 = search_pool(id2, pool, n_inpool);

		assert(op1 != NULL);
		assert(op2 != NULL);

 		n->_dot_id = dot_shape(n->_id, "else if");
		// connect with the previous node
		dot_link_dt(n->prev->_dot_id, n->_dot_id);

		op1->prev = n;
		op2->prev = n;
		op1->to_dot(op1);
		// op1 is a vardecl, it should be emiting its name
		op2->to_dot(op2);

	} else
	{
		sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d op 2: @%d ", &id1, &id2, &id3);

		node *op1 = search_pool(id1, pool, n_inpool);
		node *op2 = search_pool(id2, pool, n_inpool);
		node *op3 = search_pool(id3, pool, n_inpool);

		assert(op1 != NULL);
		assert(op2 != NULL);
		assert(op3 != NULL);

		if(op1->_ntype == goto_expr)
		{
		// do(){}while;
		// while(){}
			DEBUG("cond_expr: unexpected goto_expr in a if statement");
			exit(0);		
		}
		else
		{
			// if(){}else{}

 			n->_dot_id = dot_shape(n->_id, "else if");
			// connect with the previous node
			dot_link_dt(n->prev->_dot_id, n->_dot_id);

			op1->prev = n;
			op2->prev = n;
			op3->prev = n;
			op1->to_dot(op1);
			// op1 is the comparsion
			op2->to_dot(op2);
			if(op3->_ntype == cond_expr)
				op3->to_dot = else_to_dot;
			op3->to_dot(op3);

		}
	}
	free(op);
}

static void function_to_dot(node *n)
{
	int name_id;
	node *name;
	int value[128];

	sscanf(n->_inner, " name: @%d", &name_id);	
	name = search_pool(name_id, pool, n_inpool);
	assert(NULL != name);

	if(name->_ntype == identifier_node)
	{
		sscanf(name->_inner, " strg: %s ", value);
		n->_dot_id = dot_shape(n->_id, value);
		dot_link(n->prev->_dot_id, n->_dot_id);
	}
	else
	{
		DEBUG("Not a function pointer");
		DEBUF("%s", n->_inner);
	}

}

static void plus_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "plus_expr");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void call_to_dot(node *n)
{
	node *fn;
	node *addr;
	node *name;
	node **argu;
	char label[64] = "call ";
	char func_name[48];
	int fn_id, addr_id, name_id;
	int num_arg;

/*
 * format of call_expr
 * type: @3	fn  : @xxxx	0   : @xxxx	1  : @xxxx	....
 * */

	// first obtain the name of the function
	sscanf(n->_inner, " type: %*s fn  : @%d ", &addr_id);
	addr = search_pool(addr_id, pool, n_inpool);
	sscanf(addr->_inner, " type: %*s op 0: @%d", &fn_id);
	fn = search_pool(fn_id, pool, n_inpool);
	sscanf(fn->_inner, " name: @%d ", &name_id);
	name = search_pool(name_id, pool, n_inpool);
	sscanf(name->_inner, " strg: %s ", func_name);

	strcat(label, func_name);

	n->_dot_id = dot_shape(n->_id, label);

	// obtain the argument list of the function
	// first decide num if arguments
	char *inner = n->_inner;
	int pos = strlen(inner)-1;
	while(pos)
	{
		if(*(inner+pos) == ':')
		{
			// the last node
			pos--;
			while(1)
			{
				if(*(inner+pos) != ' ')
					break;
				pos--;
			}
			while(1)
			{
				if(*(inner+pos) != ' ')
				{
					pos--;
				}else
					break;
			}
			break;
		}
		pos--;
	}
	pos++;
	if((*(inner+pos) >=48) && (*(inner + pos)<=57))
	{
		node *temp; 
		int arg_id;

		// if there is any argument
		sscanf(inner+pos, "%d ", &num_arg);

		argu = calloc(num_arg+1, sizeof(node *));
		pos += 4;
		do
		{
			// obtain node id of arguments
			sscanf(inner+pos+3, "%d", &arg_id);
			do{	
				pos--;
			}while(*(inner+pos)!= ':');
	
			temp = search_pool(arg_id, pool, n_inpool); 
			*(argu + num_arg) = temp; 
			num_arg--;

			temp->prev = n;
			temp->to_dot(temp);
		}while(num_arg+1);

	}
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void post_inc_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "post_inc");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);	
}

static void post_dec_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "post_dec");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);	
}

static void pre_inc_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "pre_inc");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);	
}

static void pre_dec_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "pre_dec");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);	
}

static void binary_to_dot(node *n, char*type)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, type);
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void parm_decl_to_dot(node *n)
{
	int name_i;
	node *name_n;
	char name_c[32];
	sscanf(n->_inner, " name: @%d ", &name_i);
	name_n = search_pool(name_i, pool, n_inpool);
	
	sscanf(name_n->_inner, " strg: %s ", name_c);

	n->_dot_id = dot_shape(n->_id, name_c);
	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void var_decl_to_dot(node *n)
{
	int name_i;
	node *name_n;
	char name_c[32];
	sscanf(n->_inner, " name: @%d ", &name_i);
	name_n = search_pool(name_i, pool, n_inpool);
	
	sscanf(name_n->_inner, " strg: %s ", name_c);

	n->_dot_id = dot_shape(n->_id, name_c);
	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void indirect_ref_to_dot(node *n)
{
	// this function is called as the left hand of a modification
	// when left hand is referencing a ref
	int var_id;
	node *var;

	// indirect ref only have one op
	sscanf(n->_inner, " type: @%*d op 0: @%d", &var_id );
	var = search_pool(var_id, pool, n_inpool);

	n->_dot_id = dot_shape(n->_id, "*");
	var->prev = n;
	var->to_dot(var);
	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void integer_cst_to_dot(node *n)
{
	int value;
	char *buffer = calloc(1, 6);
	sscanf(n->_inner, " %*s %*s int: %d", &value);
	sprintf(buffer, "%d", value);
	n->_dot_id = dot_shape(n->_id, buffer);
	dot_link(n->prev->_dot_id, n->_dot_id);
}

static void div_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "div");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void mod_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "mod");
	n->_dot_id = t_dot_id;

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void mult_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "mult");
	n->_dot_id = t_dot_id;
	//

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void modify_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "modify");
	n->_dot_id = t_dot_id;
	//

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);


	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void truth_orif_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "||");
	n->_dot_id = t_dot_id;
	//

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}


static void truth_andif_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	assert(op1 != NULL);
	assert(op2 != NULL);
	int t_dot_id = dot_shape(n->_id, "&&");
	n->_dot_id = t_dot_id;
	//

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void convert_to_dot(node *n)
{
	int op_id, type_id, name1_id, name2_id;
	node *op;
	node *type;
	node *name1;
	node *name2;
	char sign_s[16];
	char strg[64];
	char name[64];

	sscanf(n->_inner, " type: @%d op 0: @%d", &type_id, &op_id);

	type = search_pool(type_id, pool, n_inpool);
	op = search_pool(op_id, pool, n_inpool);

	sscanf(type->_inner, " %*[^s] %*[^s] sign: %s", sign_s);
	if(*sign_s == 's')
	{
		sscanf(type->_inner, " name: @%d ", &name1_id);
		name1 = search_pool(name1_id, pool, n_inpool);
		sscanf(name1->_inner, " name: @%d ", &name2_id);
		name2 = search_pool(name2_id, pool, n_inpool);	
		sscanf(name2->_inner, " strg: %s ", strg);

		name2->prev = n->prev;
		sprintf(name, "convert_expr (%s)", strg);

		name2->_dot_id = dot_shape(name2->_id, name);
		op->prev = name2;
		op->to_dot(op);
		dot_link(name2->prev->_dot_id, name2->_dot_id);
	}
	else 
	{
		sprintf(strg, "unsigned int");
		n->_dot_id = dot_shape(n->_id, strg);
		op->prev = n;
		op->to_dot(op);
		dot_link(n->prev->_dot_id, n->_dot_id);
	}

}

static void switch_to_dot(node *n)
{
	// @xxx   switch_expr      type: @3       cond: @1882    body: @1883
	// body should be a statement_list
	int cond_id, body_id;
	node *cond;
	node *body;

	sscanf(n->_inner, " type: %*s cond: @%d body: @%d", &cond_id, &body_id);
	cond = search_pool(cond_id, pool, n_inpool);
	body = search_pool(body_id, pool, n_inpool);

	n->_dot_id = dot_shape(n->_id, "switch");

	cond->prev = n;
	cond->to_dot(cond);


	body->prev = n;

	body->to_dot(body);
	
	dot_link_dt(n->prev->_dot_id, n->_dot_id);

}

static void case_label_to_dot(node *n)
{
	int low_id, name_id;
	node *low;
	//node *name;

	if(strlen(n->_inner) < strlen("type: @151     name: @1921    low : @1893"))
	{

		n->_dot_id = dot_shape(n->_id, "default:");
	}
	else{
		sscanf(n->_inner, " %*s %*s name: @%d low : @%d ", &name_id, &low_id);
		low = search_pool(low_id, pool, n_inpool);
	
		n->_dot_id = dot_shape(n->_id, "case");
	
	low->prev = n;
	low->to_dot(low);
	}
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void pointer_plus_to_dot(node *n)
{
	int id1, id2;
	sscanf(n->_inner, "%*s @%*d op 0: @%d op 1: @%d ", &id1, &id2);

	node *op1 = search_pool(id1, pool, n_inpool);
	node *op2 = search_pool(id2, pool, n_inpool);

	n->_dot_id = dot_shape(n->_id, "poiner_plus");

	assert(op1 != NULL);
	assert(op2 != NULL);

	op1->prev = n;
	op2->prev = n;
	op1->to_dot(op1);
	// op1 is a vardecl, it should be emiting its name
	op2->to_dot(op2);

	// connect with the previous node
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static void ret_to_dot(node *n)
{
	int expr_id;
	node *expr;

	n->_dot_id = dot_shape(n->_dot_id, "return");
	if(strlen(n->_inner)>21)
	{
	sscanf(n->_inner, " %*s %*s expr: @%d", &expr_id);
	expr = search_pool(expr_id, pool, n_inpool);

	expr->prev = n;
	expr->to_dot(expr);	
	}
	dot_link_dt(n->prev->_dot_id, n->_dot_id);
}

static char  
peek(void)
{
	// peek next
	// eat \n
	int next = fgetc(fp);
	ungetc(next, fp);

	return (char)next;
}			

static void 
stub_to_dot(node *n)
{
	// TODO
	DEBUG(Stub);
	DEBUF("%d", n->_id);
	DEBUF("%s", n->_inner);
}

static void  
eval_ntype(char *buffer, node *n)
{
	// eat the buffer
	// feed the node
	char node_type[32];
	assert(buffer != NULL);
	assert(n != NULL);

	sscanf(buffer, "@%d %s ", &n->_id, node_type);

	strcpy(buffer, buffer + 8 + strlen(node_type));

	n->_ntype = str2node(node_type, n);
	strcpy(n->_inner, buffer);
}

static NODE_TYPE str2node(char *node_type, node *n)
{
	NODE_TYPE _t;
	switch(*node_type)
	{
		case 'u':
			_t = union_type;
			break;
		case 'g':
			switch(*(node_type+1))
			{

				case 't':
					_t = gt_expr;
					n->to_dot = gt_to_dot;
					break;
				case 'e':
					_t = ge_expr;
					n->to_dot = ge_to_dot;
					break;
				case 'o':
					_t = goto_expr;
					n->to_dot = goto_to_dot;
					break;
				default:
					fprintf(stderr, "Unknown node type:%s\n", node_type);
					fflush(stderr);
					exit(0);
			}
			break;
		case 'l':
			switch(*(node_type+1))
			{
				case 't':
					_t = lt_expr;
					n->to_dot = lt_to_dot;
					break;
				case 'e':
					_t = le_expr;
					n->to_dot = le_to_dot;
					break;
				case 'a':
					_t = label_expr;
					n->to_dot = label_to_dot;
					break;
				case 'l':
					_t = lshift_expr;
					n->to_dot = lshift_to_dot;
					break;
				default:
					fprintf(stderr, "Unknown node type:%s\n", node_type);
					fflush(stderr);
					exit(0);
			}
			break;
		case 's':
			switch(*(node_type+2))
			{
				case 'a':
					_t = statement_list;
					n->to_dot = sub_stmt_to_dot;
					break;
				case 'r':
					_t = string_cst;
					n->to_dot = string_cst_to_dot;
					break;
				case 'i':
					_t = switch_expr;
					n->to_dot = switch_to_dot;
					break;
				case 'v':
					_t = save_expr;
					break;
				default:
					DEBUF("Unknown node type: %s", node_type);
			}
			break;
		case 'i':
			switch(*(node_type+8))
			{
				case 'e':
	 				_t = identifier_node;
					break;
				case 'c':
					_t = integer_cst;
					n->to_dot = integer_cst_to_dot;
					break;
				case 't':
					_t = integer_type;
					break;
				case '_':
					_t = indirect_ref;
					n->to_dot = indirect_ref_to_dot;
					break;
				default:
					DEBUF("Unknown node type: %s", node_type);
			}
			break;
		case 'n':
			switch(*(node_type+2))
			{
				case '_':
					_t = ne_expr;
					n->to_dot = ne_to_dot;
					break;
				case 'p':
					_t = nop_expr;
					n->to_dot = nop_to_dot;
					break;
				case 'g':
					_t = negate_expr;
					n->to_dot = negate_to_dot;
					break;
				default:
					DEBUF("Unknown node type: %s", node_type);
					exit(0);
			}
			break;
		case 'e':
			switch(*(node_type+1))
			{
				case 'q':
					_t = eq_expr;
					n->to_dot = eq_to_dot;
					break;
				case 'n':
					_t = enumeral_type;
					break;
				case 'r':
					_t = error_mark;
				default:
					DEBUF("Unknown node type: %s", node_type);
					exit(0);
			}
			break;
		case 'p':
			switch(*(node_type + 4))
			{
				case 't':
					if(*(node_type+8) == 'p')
					{
						_t = pointer_plus_expr;
						n->to_dot = pointer_plus_to_dot;
					}
					else
					{
						_t = pointer_type;
					}
					break;
				case 'n':
					_t = preincrement_expr;
					n->to_dot = pre_inc_to_dot;
					break;
				case 'e':
					_t = predecrement_expr;
					n->to_dot = pre_dec_to_dot;
					break;
				case 'i':
					if(*(node_type+5) == 'c')
					{
						_t = predict_expr;
						n->to_dot = predict_to_dot;
					}
					else
					{
						_t = postincrement_expr;
						n->to_dot = post_inc_to_dot;
					}
					break;
				case 'd':
					_t = postdecrement_expr;
					n->to_dot = post_dec_to_dot;
					break;
				case '_':
					if(*(node_type + 5) == 'e')
					{
						_t = plus_expr;
						n->to_dot = plus_to_dot;
					}else if(*(node_type + 5) == 'd')
					{
						_t = parm_decl;
						n->to_dot = parm_decl_to_dot;
					}
					break;
				default:
					fprintf(stderr, "Unknown node type:%s\n", node_type);
					fflush(stderr);
					exit(0);
			}
			break;
		case 'r':
			switch(*(node_type+2))
			{
				case 's':
 					_t = result_decl;
					break;
				case 't':
					_t = return_expr;
					n->to_dot = ret_to_dot;
					break;
				case 'h':
					_t = rshift_expr;
					n->to_dot = rshift_to_dot;
					break;
				case 'a':
					_t = real_type;
					break;
				case 'c':
					_t = record_type;
					break;
				case 'f':
					_t = reference_type;
					break;
				default:
					DEBUF("Unknown node type: %s", node_type);
					exit(0);
			}
			break;		
		case 'd':
			_t = decl_expr;
			break;
		case 'm':
			switch(*(node_type+1))
			{
				case 'o':
					_t = modify_expr;
					n->to_dot = modify_to_dot;
					break;
				case 'i':
					_t = minus_expr;
					n->to_dot = minus_to_dot;
					break;
				case 'u':
					_t = mult_expr;
					n->to_dot = mult_to_dot;
					break;
				default:
						DEBUF("Unknown node type %s", node_type);
						exit(0);
			}
			break;
		case 'v':
			_t = var_decl;
			n->to_dot = var_decl_to_dot;
			break;
		case 'f':
			if(*(node_type+9) == 'd')
			{
				_t = function_decl;
				n->to_dot = function_to_dot;
			}
			else if(*(node_type+9) == 't')
				_t = function_type;
			break;
		case 't':
			if(*(node_type+1) == 'r')
			{
				switch(*(node_type+6))
				{
					case 'd':
						_t = trunc_div_expr;
						n->to_dot = div_to_dot;
						break;
					case 'm':
						_t = trunc_mod_expr;
						n->to_dot = mod_to_dot;
						break;
					case 'i':
						_t = tree_list;
						break;
					case 'a':
						_t = truth_andif_expr;
						n->to_dot = truth_andif_to_dot;
						break;
					case 'o':
						_t = truth_orif_expr;
						n->to_dot = truth_orif_to_dot;
						break;
					default:
						DEBUF("Unknown node type %s", node_type);
						exit(0);
				}
			}
			else if(*(node_type+9) == 'y')
				_t = type_decl;
			break;
		case 'c':
			switch(*(node_type+3))
			{
				case 'l':
					_t = call_expr;
					n->to_dot = call_to_dot;
					break;
				case 'p':
					if(*(node_type+4)=='l')
					{
						_t = complex_type;
					}
					else if(*(node_type+4)=='o')
					{
						_t = component_ref;
						n->to_dot = component_to_dot;
					}
					break;
				case 'd':
					_t = cond_expr;
					n->to_dot = cond_to_dot;
					break;
				case 'v':
					_t = convert_expr;
					n->to_dot = convert_to_dot;
					break;
				case 's':
					_t = constructor;
					break;
				case 'e':
					_t = case_label_expr;
					n->to_dot = case_label_to_dot;
					break;
				default:
					DEBUF("Unknown node type: %s", node_type);
					exit(0);
			}
			break;
		case 'a':
			switch(*(node_type + 1))
			{
				case 'r':
					_t = array_type;
					break;
				case 'd':
					_t = addr_expr;
					n->to_dot = addr_to_dot;
					break;
				default:
					DEBUF("Unknown node type: %s", node_type);
			}
			break;
		case 'b':
			if(*(node_type+1) == 'o')
				_t = boolean_type;
			else if(*(node_type+1) == 'i')
			{
				_t = bind_expr;
				n->to_dot = bind_to_dot;
			}	
			break;

		default:
			_t = -1;
			fprintf(stderr, "Unknown node type:%s\n", node_type);
			fflush(stderr);
	}

	// TODO
	if(n->to_dot == NULL)
		n->to_dot = stub_to_dot;
	return _t;
}

static void dump_node(node *n)
{
	assert(n!=NULL);
	printf("ID: %d\ntype: 0x%x\n%s\n", n->_id, n->_ntype, n->_inner);
	fflush(stdout);
}

void 
open_file(char **argv)
{
        if((fp =fopen(argv[1], "r")) == NULL)
        {
                fprintf(stderr, "Usage: ./tu_eater <file dir>\n");
                fflush(stderr);
                exit(0);
        }
}

static int 
check_inner(node *n, char *name)
{
	assert(n != NULL);
	assert(name != NULL);
	char value[4+strlen(name)];
	sprintf(value, "%s ", name);
	if(NULL == strstr(n->_inner, value))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static node** read_statement(node *n)
{
	// n is the identifier node of the scope

	node **node_list = (node **)calloc(NUM_NODE, sizeof(node*));
	node *temp = n;
	int counter = 1;

	assert(NULL != temp);

	// we make first element in the list be the statement_list node
	while(temp->_ntype != statement_list)
	{
		temp = get_next(temp, pool, n_inpool);
	}
	*node_list = temp;	
	while(temp->_ntype != return_expr)
	{
		temp = get_next(temp, pool, n_inpool);
		*(node_list + counter) = temp;
		counter++;
	}

	// DEBUG
	return node_list;
}

static node **parse_stmt(node **node_list)
{
	// first parse the statement
	int num_node;
	int expr[NUM_EXPR];	// we assume there is NUM_EXPR=64 expr
	int counter = 0;
	node *statement = *node_list;	// first stmt in the list is the statement_list
	char *inner = statement->_inner;

	assert(NULL != statement);
	assert(NULL != node_list);
	while(1)
	{
		if(*(inner + counter) == '\0')
			break;
		if(*(inner+counter) == '@')
		{
			sscanf(inner + counter - 6, "%d ", &num_node);
			sscanf(inner + counter + 1, "%d", &expr[num_node]);
		}
		counter++;	
	}

	node **expr_list = (node **)calloc(num_node+2, sizeof(node*));
	// find the node from the node_list and add to the expr_list

/*	*expr_list = search_pool(expr[0], pool, n_inpool);
	for(int i=1; i<num_node+1;i++)
	{
	*(expr_list+i) = search_pool(expr[i], pool, n_inpool);
	(*(expr_list+i))->prev = *(expr_list+i-1);
	}*/
	counter = 1;
	int nelms = 1;
	node *temp = *(node_list+counter);

	DEBUF("%s", (**node_list)._inner);
	do
	{
		for(int i=0; i<= num_node; i++)
		{
			if(temp->_id == expr[i])
			{
				// Add local decl to local symboltable
				if(temp->_ntype == decl_expr)
					;
				else
				{
					*(expr_list+nelms) = temp;
					// set prev node
					(**(expr_list+nelms)).prev = *(expr_list+nelms-1);
					nelms++;			
				}
			}
		}
		counter++;
		temp = *(node_list+counter);
	}while(temp != NULL);
	// the first node in expr_list points to the statement_list
	*expr_list = statement;

	// there are dependencies in the nodes
	// we are expecting to feed the dependencies
	// however, it is not necessary to feed all the dependencies
		

	return expr_list; 
}

static void 
dump_list(node **node_list, char* scope, node *start_node)
{
	node *temp;
	node **expr_list = parse_stmt(node_list);	
	int counter = 1;
	assert(expr_list != NULL);

	node *scope_ident = (node *)calloc(1, sizeof(node));
	scope_ident->_ntype = scope_start;

	scope_ident->_id = start_node->_id;

	*expr_list = scope_ident;


	// the first node is the declaration of the scope
	
	// return the node list of all the statements
	// dump info is fed already

	temp = *(expr_list + counter);
	// connect expr list w
	temp->prev = *expr_list;
	temp->prev->_dot_id = 1;
	// scope_start dont need to to_dot
	// as we have the emit_header function
	do
	{	

		temp->to_dot(temp);
		counter++;
		temp = *(expr_list + counter);
	}while(temp != NULL);
	// following the statement list
	// the to_dot funtion of the expr is called

	free(expr_list);
	free(scope_ident);
}

static void 
emit_header(char *scpe, int start_id)
{
	fprintf(stdout, "digraph AST {\n");
	fprintf(stdout, "  graph [fontname=\"Times New Roman\",fontsize=10];\n");
	fprintf(stdout, "  node  [fontname=\"Courier New\",fontsize=10];\n");
	fprintf(stdout, "  edge  [fontname=\"Times New Roman\",fontsize=10];\n\n");
	
	fprintf(stdout, "  node%d [label=\"scope %s\",shape=box];\n", acquire_id(), scpe);
	inc_id();
	fflush(stdout);
}

static void eval_statement(node *n, char *scope)
{
	// n contains name of the scope
	node **node_list;
	node *temp;

	int ident_id = n->_id;
	char value[64];
	sprintf(value, "name: @%d", ident_id);
	temp = *pool;

	while(1)
	{
		temp = get_next(temp, pool, n_inpool);
		if(temp->_ntype == function_decl)
		{
			if(check_inner(temp, value))
			{
				break;
			}
		}
	};

	char *body = strstr(temp->_inner, "body:");
	int body_id;
	sscanf(body, "body: @%d", &body_id);
	temp = search_pool(body_id, pool, n_inpool);
	int bind_id;
	sscanf(temp->_inner, " %*[^b] body: @%d", &bind_id);
	n = search_pool(bind_id, pool, n_inpool);
	if(n->_ntype != statement_list)
	{
		DEBUF("node type %d unexpected", n->_ntype);
		exit(0);
	}

	int num_op = 0;
	int op[NUM_EXPR];
	int counter=0;
	char *inner = n->_inner;
	while(1)
	{
//	op = read_op_inner(n->_inner, &num_op);	
		if(*(inner+counter) == '\0')
			break;
		if(*(inner+counter) == '@')
		{
			sscanf(inner+counter-6, "%d ", &num_op);
			sscanf(inner+counter+1, "%d ", &op[num_op]);		
		}
		counter++;
	}
	num_op++;
	

	node_list = calloc(num_op+1, sizeof(node*));
	*node_list = n;
	n->_dot_id = 1;
	counter = 1;

	for(int i=1; i <= num_op; i++)
	{
		temp = search_pool((*(op+i-1)), pool, n_inpool);
		if(temp->_ntype == decl_expr)
			continue;
		*(node_list+counter) = temp;
		(*(node_list+counter))->prev = (*(node_list+counter-1));
		counter++;
	}

	for(int i=1; i < counter ; i++)
	{
		temp = *(node_list+i);
		(*(node_list+i))->to_dot(*(node_list+i));
	}

	free(node_list);
	return ;
	node_list = read_statement(n);
	// read up coming statements utill return
	dump_list(node_list, scope, n);

	// n is the start of the scope
	// to_dot is called only in dump_list
	// dump_node is used for debugging
	
	free(node_list);
}

size_t 
get_filesize(void)
{
	// only call this function before you move the ptr to file position
        fseek(fp, 0L, SEEK_END);
        size_t size =(size_t)ftell(fp);
        rewind(fp);
	return size;
}

void 
eval_file(char *name)
{
	node *n;// = (node *)calloc(1, sizeof(node)); 
	// n will be the root of ast
	// identifier node of main function, in our example
	int target_id = INT_MAX;
	// in this case we only have one target
	assert(NULL != name);
	char *scp_name = calloc(16+strlen(name), sizeof(char));
	sprintf(scp_name, "strg: %s", name);

	while(1)
	{
		n = eval_node();
	
		if(n == NULL)
		// EOF
			break;

		if(check_inner(n, scp_name) && n->_ntype == identifier_node)
		{
			target_id = n->_id;
		}
	}
	if(target_id == INT_MAX)
	{
		DEBUF("Scope %s not found.", name);
		exit(0);
	}

	node *target_n = search_pool(target_id, pool, n_inpool);
	// eval a scope with name "main"
	assert(NULL != target_n);
	emit_header(name, target_n->_id);
	eval_statement(target_n, name);



        if( fclose(fp) == EOF)
        {
                fprintf(stderr, "File close error:%s\n");
                fflush(stderr);
                exit(0);
        }

	fprintf(stdout, "} ");
	fflush(stdout);
}



