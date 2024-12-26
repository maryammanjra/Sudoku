#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

int board[9][9]; 

typedef struct{
    int row;
    int column;
    int valid;
} position;

void *validate_row(void *parameter){
    position *coordinates = (position *)(parameter);
    int row = coordinates->row;
    int bitmap[9];

    for(int i = 0; i < 9; i++){
        bitmap[i] = 0;
    }

    int value; 
    for(int j = 0; j < 9; j++){
        value = board[row][j];
        if(bitmap[value - 1] == 0){
            bitmap[value - 1] = 1;
        }
        else{
            coordinates->valid = 0;
            break;
        }
    }
    pthread_exit(0);
}

void *validate_column(void *parameter){
    position *coordinates = (position *)(parameter);
    int col = coordinates->column;
    int bitmap[9];

    for(int i = 0; i < 9; i++){
        bitmap[i] = 0;
    }

    int value; 
    for(int i = 0; i < 9; i++){
        value = board[i][col];
        if(bitmap[value - 1] == 0){
            bitmap[value - 1] = 1;
        }
        else{
            coordinates->valid = 0;
            break;
        }
    }
    pthread_exit(0);
}

void *validate_subgrid(void *parameter){
    position *coordinates = (position *)(parameter);
    int col = coordinates->column;
    int row = coordinates->row;
    int bitmap[9];

    for(int i = 0; i < 9; i++){
        bitmap[i] = 0;
    }

    int value;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
           value = board[row + i][col + j];
           if(bitmap[value - 1] == 0){
               bitmap[value - 1] = 1;
            }
           else{
                coordinates->valid = 0;
                break;
            } 
        }
    }
    pthread_exit(0);
}

int main(void){
    
    pthread_attr_t attr;
    pthread_t tid_row;
    pthread_t tid_col;
    pthread_t sub_grid[9];
    position *subgrid_returns[9];

    pthread_attr_init(&attr);

    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            board[i][j] = j + 1;
        }
    }

    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            printf("%d|", board[i][j]);
        }
        printf("\n");
    }

    position *rows = malloc(sizeof(position));
    position *columns = malloc(sizeof(position));
    position *subgrids = malloc(sizeof(position));

    rows->row = 0;
    rows->column = 0;
    rows->valid = 1;

    while(rows->row < 9){
        pthread_create(&tid_row, &attr, validate_row, rows);
        pthread_join(tid_row, NULL);
        if(rows->valid == 0){
            printf("Invalid row %d\n", rows->row);
        }
        rows->row++;
        rows->valid = 1;
    }

    columns->column = 0;
    columns->row = 0;
    columns->valid = 1;

    while(columns->column < 9){
        pthread_create(&tid_col, &attr, validate_column, columns);
        pthread_join(tid_col, NULL);
        if(columns->valid == 0){
            printf("Invalid column %d\n", columns->column);
        }
        columns->column++;
        columns->valid = 1;
    }

    subgrids->column = 0;
    subgrids->row = 0;
    subgrids->valid = 1;

    int row_count = 0; 
    int col_count = 0; 

    for(int i = 0; i < 9; i++){
        position *subgrid = malloc(sizeof(position));
        subgrid_returns[i] = subgrid;
        subgrid->row = (row_count % 3) * 3;
        subgrid->column = (col_count % 3) * 3;
        subgrid->valid = 1;

        pthread_create(&sub_grid[i], &attr, validate_subgrid, subgrid);

        if(i % 3 == 0){
            row_count++;
        }
        col_count++;
    }

    for(int i = 0; i < 9; i++){
        pthread_join(sub_grid[i], NULL);
    }

    for(int i = 0; i < 9; i++){
        printf("Valid: %d\n", subgrid_returns[i]->valid);
    }
}