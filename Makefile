# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#              #
#    Updated: 2017/02/24 15:22:37 by tettouat         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ft_strace

FLAGS		=	-Wall -Wextra -Werror -O3

SRCDIR		=	srcs/

INCDIR		=	includes/

OBJDIR		=	objs/

SRC			=	format.c \
				log.c \
				main.c \
				path.c \
				peek.c \
				strace.c \
				syscall.c \
				tools.c

SRCS =	$(addprefix $(SRCDIR), $(SRC))

OBJ	= $(addprefix $(OBJDIR), $(SRC:.c=.o))

all:		$(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@mkdir -p $(dir $(OBJ))

$(OBJDIR)%.o : $(SRCDIR)%.c | $(OBJDIR)
	@gcc $(FLAGS) -c $< -o $@ -I $(INCDIR)

$(NAME):	$(OBJDIR) $(OBJ)
	@gcc $(FLAGS) -o $(NAME) $(OBJ)

clean:
	if [ -d $(OBJDIR) ]; then rm -r $(OBJDIR); fi

fclean:	clean
	@rm -f $(NAME)

re:	fclean all


.PHONY: fclean clean re 

.SILENT: clean