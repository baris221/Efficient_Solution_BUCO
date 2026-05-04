# Makefile for Pareto Multi-Objective Knapsack DP V2
CC = gcc
CFLAGS = -Wall -O3

SRCS = main.c read_instance.c knapsackDP.c NDTree/treeNDS.c NDTree/tabSol.c NDTree/dominance.c NDTree/tabNDS.c
OBJS = $(SRCS:.c=.o)

TARGET = mainDP

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)