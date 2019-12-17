#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DECK_SIZE 52

typedef struct
{
	const char *suit;
	const char *val;
} card;

card *cards, *top_card;
card *dealer_hand, *player_hand;

int allocated_hand_count = 10, bet = 0, cash = 100;
int dealer_count, player_count, d_depth;

void clear_memory()
{
	free( cards );
	free( dealer_hand );
	free( player_hand );
}

int next_card( card* );
void deal_cards()
{
	int cnt = 0;
	player_hand[cnt].suit = top_card->suit;
	player_hand[cnt].val = top_card->val;
	player_count++;

	next_card( top_card );

	dealer_hand[cnt].suit = top_card->suit;
	dealer_hand[cnt].val = top_card->val;
	dealer_count++;

	printf( "Dealer reveals %s of %s\n", dealer_hand[0].val, dealer_hand[0].suit );
}

int sum( card*, int );
int game_over()
{
	int player_sum = sum( player_hand, player_count );
	if( player_sum > 21 ) { printf( "You bust!\n" ); }
	return player_sum > 21;
}

void hit()
{
	if( ++player_count < allocated_hand_count )
	{
		player_hand[player_count - 1].suit = top_card->suit;
		player_hand[player_count - 1].val = top_card->val;
		next_card( top_card );
	}
}

void hit_dealer()
{
	if( ++dealer_count < allocated_hand_count )
	{
		dealer_hand[dealer_count - 1].suit = top_card->suit;
		dealer_hand[dealer_count - 1].val = top_card->val;
		next_card( top_card );
	}
}

int next_card( card* c )
{
	if( ++d_depth >= DECK_SIZE )
	{ return 0; }

	c->suit = cards[d_depth].suit;
	c->val = cards[d_depth].val;
	return 1;
}

void place_bet()
{
	printf( "How much would you like to bet? $" );
	scanf_s( "%d", &bet );

	if( bet > cash )
	{
		printf( "Too high! You may only bet at most $%d\n", cash );
		place_bet();
	}

	cash -= bet;
}

int setup()
{
	srand( time( NULL ) );
	cards = (card*)malloc( DECK_SIZE * sizeof( card ) );
	if( !cards ) { return 0; }

	dealer_hand = (card*)malloc( allocated_hand_count * sizeof( card ) );
	if( !dealer_hand )
	{
		free( (void*)cards );
		return 0;
	}

	player_hand = (card*)malloc( allocated_hand_count * sizeof( card ) );
	if( !player_hand )
	{
		free( (void*)cards );
		free( (void*)dealer_hand );
		return 0;
	}

	d_depth = 0;
	int x = 0, i = 0, j = 0;
	top_card = &cards[d_depth];
	const char* suits[4] = { "Spades", "Hearts", "Clubs", "Diamonds" };
	const char* vals[13] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
	while( x < DECK_SIZE )
	{
		cards[x].suit = suits[i++];
		cards[x].val = vals[j++];
		if( i >= 4 ) { i = 0; }
		if( j > 12 ) { j = 0; }
		x++;
	}

	dealer_count = 0;
	player_count = 0;

	return 1;
}

void show_cards( card *c, char *whose, int cnt )
{
	int i;

	printf( "\n%s's Hand:\n", whose );

	for( i = 0; i < cnt; i++ )
	{
		printf( "\t%s of %s\n", c[i].val, c[i].suit );
	}
}

void shuffle_deck( card *cards, int shuffles )
{
	int i, j;
	if( shuffles == 0 )
	{ return; }

	for( i = 0; i < DECK_SIZE; i++ )
	{
		j = ( rand() % ( DECK_SIZE - i ) ) + i;
		card t = cards[i];
		cards[i] = cards[j];
		cards[j] = t;
	}

	shuffle_deck( cards, shuffles - 1 );
}

int sum( card *hand, int cnt )
{
	int i, ace = 0, csum = 0;
	for( i = 0; i < cnt; i++ )
	{
		const char *val = hand[i].val;
		if( atoi( val ) == 0 )
		{
			if( val == "A" )
			{
				ace++;
				csum += 11;
			}
			else { csum += 10; }
		}
		else { csum += atoi( val ); }
	}

	while( ace > 0 && csum > 21 )
	{
		csum -= 10;
		ace--;
	}

	return csum;
}

int main( void )
{
	int play = 1;
	while( cash > 1 && play )
	{
		if( !setup() ) { return 1; }
		shuffle_deck( cards, 3 );

		int p_choice = 1;
		printf( "[1] Play [2] Quit " );
		scanf_s( "%d", &p_choice );
		if( p_choice == 2 ) { break; }
		system( "cls" );

		printf( "Cash Balance: $%d\n", cash );
		place_bet();
		deal_cards();

		while( !game_over() && p_choice )
		{
			show_cards( player_hand, "Player", player_count );
			printf( "\n[1] Hit [2] Hold " );
			scanf_s( "%d", &p_choice );
			if( p_choice == 1 )
			{
				system( "cls" );
				hit();
			}
			else if( p_choice == 2 )
			{
				p_choice = 0;
			}
		}

		if( !game_over() )
		{
			system( "cls" );
			printf( "\nCheck dealers hand\n" );
			while( sum( dealer_hand, dealer_count ) <= 17 && sum( dealer_hand, dealer_count ) < sum( player_hand, player_count ) )
			{ hit_dealer(); }
			show_cards( dealer_hand, "Dealer", dealer_count );

			int d_score = sum( dealer_hand, dealer_count ),
				p_score = sum( player_hand, player_count );
			printf( "\n\nPlayer's Score: %d\n", p_score );
			printf( "Dealer's Score: %d\n\n", d_score );

			if( p_score > d_score || d_score > 21 ) {
				cash += 2 * bet;
				printf( "Player Wins!\n" );
			}
			else { printf( "Dealer Wins!\n" ); }
		}

		clear_memory();
	}

	if( cash <= 0 ) { printf( "\nYou managed to go completely broke in Vegas... Big surprise!\n" ); }
	else
	{
		printf( "\nSo, you cut your losses and took your sorry ass home for a measely $%d?", cash );
		printf( "I knew you were too chicken shit to play with the big boys here in Vegas...\n" );
	}

	return 0;
}
