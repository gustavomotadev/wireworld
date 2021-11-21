#include <stdio.h>
#include <stdlib.h>

//#define board_width 32
//#define board_height 36
//#define n_cells board_width*board_height
#define EMPTY 0b10000000
#define COPPER 0b01000000
#define TAIL 0b00100000
#define HEAD 0b00000001

//neighbour values
#define N1(x,w) (*(x-w-1))
#define N2(x,w) (*(x-w))
#define N3(x,w) (*(x-w+1))
#define N4(x,w) (*(x-1))
#define N5(x,w) (*(x+1))
#define N6(x,w) (*(x+w-1))
#define N7(x,w) (*(x+w))
#define N8(x,w) (*(x+w+1))

typedef unsigned char cell;

unsigned char not_negative_int(char * str);
int get_argument(int argc, char *argv[]);
char cell_to_text(cell c);
void print_cells(cell * cells, unsigned int width, unsigned int height);
void init_automaton();
void iterate(unsigned long long int generations);

unsigned int board_width = 0;
unsigned int board_height = 0;
unsigned int n_cells = 0;

cell ** heads;
cell ** tails;
cell ** new;
cell * cells;

int n_heads = 0, n_tails = 0, n_new = 0;
unsigned long long int current_gen = 0;

void allocate_and_fill(char * file_name)
{
    unsigned int i;
    unsigned int scan_uint;
    FILE *csv_ptr;

    csv_ptr = fopen(file_name, "r");

    fscanf(csv_ptr, "%d,%d,", &board_width, &board_height);
    n_cells = board_width*board_height;
    printf("%d; %d; %d;\n", board_width, board_height, n_cells); //debug

    heads = (cell **) malloc(n_cells*sizeof(cell *));
    tails = (cell **) malloc(n_cells*sizeof(cell *));
    new = (cell **) malloc(n_cells*sizeof(cell *));
    cells = (cell *) malloc(n_cells*sizeof(cell));

    for (i = 0; i < n_cells; i++)
    {
        fscanf(csv_ptr, "%d,", &scan_uint);
        cells[i] = (cell) scan_uint;
    }

    fclose(csv_ptr);
}

void init_automaton()
{
    unsigned int i_cell;

    //iterate cells and fill heads table and tails table
    for (i_cell = 0; i_cell < n_cells; i_cell++)
    {
        if (cells[i_cell] == HEAD) heads[n_heads++] = &cells[i_cell];
        else if (cells[i_cell] == TAIL) tails[n_tails++] = &cells[i_cell];
    }
    //printf("HEADS: %d; TAILS: %d\n", n_heads, n_tails);
    //printf("HEADS[0]: %d; TAILS[0]: %d\n", heads[0]-&cells[0], tails[0]-&cells[0]);
}

void iterate(unsigned long long int generations)
{
    cell ** aux_ptr;
    cell * cell_ptr;

    unsigned int i_head, i_tail, i_new, sum, duplicate;
    int line, col, board_width_int;
    unsigned long long int i_gen;

    board_width_int = (int) board_width;

    //################################################################
    for(i_gen = 0; i_gen < generations; i_gen++)
    {
        //generation logic

        //for each head and for each copper around head
        n_new = 0;
        for (i_head = 0; i_head < n_heads; i_head++)
        {
            //printf("HEAD: %d\n", heads[i_head]-&cells[0]);
            for(line = -board_width_int; line <= board_width_int; line += board_width_int)
            {
                for(col = -1; col <= 1; col++)
                {
                    cell_ptr = heads[i_head]+line+col;
                    //printf("%d: %d\n", *cell_ptr, cell_ptr-&cells[0]);
                    if (*cell_ptr == COPPER) 
                    {
                        //sum how many heads are around
                        sum = (N1(cell_ptr, board_width_int) + N2(cell_ptr, board_width_int) + N3(cell_ptr, board_width_int) + 
                            N4(cell_ptr, board_width_int) + N5(cell_ptr, board_width_int) + N6(cell_ptr, board_width_int) + 
                            N7(cell_ptr, board_width_int) + N8(cell_ptr, board_width_int)) & 0x0f;
                        
                        //rule says copper only turns into head if 1 or 2
                        if (sum == 1 || sum == 2)
                        {
                            //prevent adding duplicate heads to new head array
                            duplicate = 0;
                            for (i_new = 0; i_new < n_new; i_new++)
                            {
                                if (new[i_new] == cell_ptr)
                                {
                                    duplicate = 1;
                                    break;
                                }
                            }
                            //this defines cells that should become heads
                            if (!duplicate) new[n_new++] = cell_ptr;
                        }
                    }
                }
            }
        }
        //end iteration over old heads

        //apply rules
        //head --> tail
        for (i_head = 0; i_head < n_heads; i_head++) *heads[i_head] = TAIL;
        //tail --> copper
        for (i_tail = 0; i_tail < n_tails; i_tail++) *tails[i_tail] = COPPER;
        //copper --> head (if 1 or 2 neighbouring heads present)
        for (i_new = 0; i_new < n_new; i_new++) *new[i_new] = HEAD;

        //adjust arrays
        //the old content of tails is useless because they turned into copper
        //just save the referece to memory to use as new after
        aux_ptr = tails;
        //heads are now tails, so array tails receives heads
        tails = heads;
        n_tails = n_heads;
        //array heads receives new heads
        heads = new;
        n_heads = n_new;
        //reuse space used by tails before for new heads
        new = aux_ptr;
        n_new = 0;

        current_gen++;
    }
}

unsigned char not_negative_int(char * str)
{
    unsigned int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}

int get_argument(int argc, char *argv[])
{
    int value;
    if (argc != 2) 
    {
        printf("Wrong number of arguments, pass number of generations to run.");
        return -1;
    }

    if (not_negative_int(argv[1]))
    {
        value = (int) strtol(argv[1], (char **)NULL, 10);
    }
    else
    {
        printf("Argument should be non negative integer.");
        return -1;
    }

    return value;
}

char cell_to_text(cell c)
{
    switch (c)
    {
        case EMPTY:
            return ' ';
        case COPPER:
            return '=';
        case HEAD:
            return 'H';
        case TAIL:
            return 'T';
        default:
            return '?';
    }
}

void print_cells(cell * cells, unsigned int width, unsigned int height)
{
    unsigned int i_h, i_w;
    for (i_h = 0; i_h < height; i_h++)
    {
        for (i_w = 0; i_w < width; i_w++)
        {
            putchar(cell_to_text(cells[i_h*width + i_w]));
        }
        putchar('\n');
    }
}