#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "ghksdl86..";
const char* db = "db_proj2";

int ui(MYSQL* connection, MYSQL* conn_);	//functino for user interface
int pre_sql(MYSQL* connection, MYSQL* conn_);		//function for create table and insert data
int del_sql(MYSQL* connection, MYSQL* conn_);		//functino for delete and drop table

int main(void) {

	MYSQL* connection = NULL;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		printf("Connection Succeed\n");

		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}
		
		pre_sql(connection, &conn);
		ui(connection, &conn);
		del_sql(connection, &conn);

		mysql_close(connection);
	}

	return 0;
}



int ui(MYSQL* connection, MYSQL* conn_) {
	printf("***********************************************\n");
	printf("** Sogang Package Delivery Management System **\n");
	printf("***********************************************\n\n\n");

	//while for select type
	while (1) {
		MYSQL_RES* sql_result;
		int type_num;
		MYSQL_ROW sql_row;
		printf("------- SELECT QUERY TYPES -------\n");
		printf("\t1. TYPE I\n");
		printf("\t2. TYPE II\n");
		printf("\t3. TYPE III\n");
		printf("\t4. TYPE IV\n");
		printf("\t5. TYPE V\n");
		printf("\t0. QUIT\n");
		printf("Which type of query? ");
		scanf("%d", &type_num);

		if (type_num > 5 || type_num < 0) {
			printf("TYPE number error! please choose 0~5\n");
		}
		if (type_num == 0) {
			printf("HAVE A GOOD DAY :)\n");
			return 0;
		}
		else if (type_num == 1) {
			int truck_num;
			int destroy_flag = 1;
			printf("\n---- TYPE I ----\n");
			//get truck number
			while (1) {
				printf("Input the number of truck : ");
				scanf("%d", &truck_num);

				if (truck_num == 0) break; // if get 0 go to select

				//-------------code for get destroy_Flag

				//if truck is not destroyed get truck number until get0 or destroyed truck number
				if (truck_num != 1721) {
					printf("Truck %d is not destroyed.\n\n", truck_num);
				}
				else {
					break;
				}
			}
			if (truck_num != 0) {

				//while for select subtype
				while (1) {
					int sub_num;
					printf("---- Subtypes in TYPE I ----\n");
					printf("\t1. TYPE I-1\n");
					printf("\t2. TYPE I-2\n");
					printf("\t3. TYPE I-3\n");
					printf("Which type of query? ");
					scanf("%d", &sub_num);

					//if get 0 then go to select
					if (sub_num == 0) break;

					if (sub_num > 3 || sub_num < 0) {
						printf("Wrong sub type number!\n");
					}

					if (sub_num == 1) {
						printf("** Find all customers who had a package on the truck at the time of the crash. **\n");
						const char* query = "with track_recent(package_id, tracking_id) as\
							(select package_id, max(tracking_id)\
								from tracking\
								group by package_id\
								),\
							crush_package(package_id) as\
							(select package_id\
								from track_recent natural join tracing_vehicle\
								where vehicle_kind = \'truck\' and vehicle_number = 1721\
							),\
							sender(customer_id) as\
							(select sender_id\
								from package\
						where package.package_id in(select package_id from crush_package)\
							)\
							select distinct customer_id, name\
							from customer natural join sender";
						int state = 0;

						state = mysql_query(connection, query);
						if (state != 0)
						{
							printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
							return 1;
						}
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								printf("Customer ID : %s, Name : %s\n", sql_row[0], sql_row[1]);
							}
							printf("\n");
							mysql_free_result(sql_result);
						}
					}
					else if (sub_num == 2) {
						printf("** Find all recipients who had a package on that truck at the time of the crash. **\n");
						const char* query = "with track_recent(package_id, tracking_id) as\
							(select package_id, max(tracking_id)\
								from tracking\
								group by package_id\
								),\
							crush_package(package_id) as\
							(select package_id\
								from track_recent natural join tracing_vehicle\
							),\
							receiver(customer_id) as\
							(select receiver_id\
								from package\
						where package.package_id in(select package_id from crush_package)\
							)\
							select customer_id, name\
							from customer natural join receiver";
						int state = 0;

						state = mysql_query(connection, query);
						if (state != 0)
						{
							printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
							return 1;
						}
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								printf("Recipient ID: %s, Name : %s\n", sql_row[0], sql_row[1]);
							}
							printf("\n");
							mysql_free_result(sql_result);
						}
					}
					else if (sub_num == 3) {
						printf("** Find the last successful delivery by that truck prior to the crash. **\n");
						const char* query = "with use_truck1721(package_id, tracking_id) as\
							(select package_id, tracking_id\
								from tracking natural join tracing_vehicle\
						where vehicle_kind = \'truck\' and vehicle_number = 1721\
							),\
							track_recent(package_id, tracking_id) as\
							(select package_id, max(tracking_id)\
								from tracking\
								group by package_id\
							),\
							success_pid(package_id) as\
							(select distinct package_id\
								from use_truck1721\
						where tracking_id not in(select distinct tracking_id from track_recent)\
							)\
							select content, package_id\
							from package\
						where package_id in(select package_id from success_pid)";
						int state = 0;

						state = mysql_query(connection, query);
						if (state != 0)
						{
							printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
							return 1;
						}
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								printf("Last delivery Content : %s, Package ID : %s\n", sql_row[0], sql_row[1]);
							}
							printf("\n");
							mysql_free_result(sql_result);
						}

					}
				}
			}
		}
		else if (type_num == 2) {
			printf("\n---- TYPE II ----\n");
			while (1) {
				char year[5];
				printf("** Find the customer who has shipped the most packages in the past certain year. **\n");
				printf("Which year? ");
				scanf("%s", year);

				//if get 0 then go to select
				if (!strcmp(year,"0")) {
					break;
				}
				char query[1024] = "with package_num_year(customer_id, year, num) as (select customer_id, year(yearmonth), count(package_id) from payment group by customer_id, year(yearmonth)), max_customer(customer_id, max_num) as (select customer_id, num from package_num_year where year =";
				strcat(query, year);
				strcat(query, " and num = (select max(num) from package_num_year where year = ");
				strcat(query, year);
				strcat(query, ")) select customer_id, name from customer where customer_id in(select customer_id from max_customer)");
				int state = 0;

				state = mysql_query(connection, query);
				if (state != 0)
				{
					printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
					return 1;
				}
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("Customer ID : %s, Name : %s\n", sql_row[0], sql_row[1]);
					}
					printf("\n");
					mysql_free_result(sql_result);
				}
			}
		}
		else if (type_num == 3) {
			printf("\n---- TYPE III ----\n");
			while (1) {
				int year;
				printf("** Find the customer who has spent the most money on shipping in the past certain year. **\n");
				printf("Which year? ");
				scanf("%d", &year);

				//if get 0 then go to select
				if (year == 0) {
					break;
				}
				const char* query = "with package_sum_year(customer_id, year, sum) as\
					(select customer_id, year(yearmonth), sum(cost)\
						from payment\
						group by customer_id, year(yearmonth)),\
					customer_sum_year(customer_id, sum) as\
					(select customer_id, sum\
						from package_sum_year\
				where year = 2020 and sum = (select max(sum) from package_sum_year where year = 2020))\
					select customer_id, name\
					from customer\
				where customer_id in(select customer_id from customer_sum_year)";
				int state = 0;

				state = mysql_query(connection, query);
				if (state != 0)
				{
					printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
					return 1;
				}
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("Customer ID : %s, Name : %s\n", sql_row[0], sql_row[1]);
					}
					printf("\n");
					mysql_free_result(sql_result);
				}
			}
		}
		else if (type_num == 4) {
			printf("\n---- TYPE IV ----\n");
			printf("** Find those packages that were not delivered within the promised time. **\n");
			
			const char* query = "with late_package(package_id) as\
				(select package_id\
					from predicted_time natural join delivered\
			where predicted_time < delivered_time)\
				select package_id, content\
				from package\
			where package_id in(select package_id from late_package)";
			int state = 0;

			state = mysql_query(connection, query);
			if (state != 0)
			{
				printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
				return 1;
			}
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
				{
					printf("Package ID : %s, Content : %s\n", sql_row[0], sql_row[1]);
				}
				printf("\n");
				mysql_free_result(sql_result);
			}
		}
		else if (type_num == 5) {
			printf("\n---- TYPE V ----\n");
			while (1) {
				char name[20];
				char ym[10];
				char year[10];
				char month[10];
				char output_filename[50] = "bill_";
				char *ptr;
				
				printf("** Generate the bill for each customer for the past certain month. **\n");
				printf("Customer Name : ");
				scanf("%s", name);
				if (!strcmp(name, "0")) {
					break;
				}

				printf("Which month(YYYY-MM)? ");
				scanf("%s", ym);
				if (!strcmp(ym, "0")) {
					break;
				}

				printf("Generate Bill..\n");

				//tokenize year and month

				ptr = strtok(ym, "-");
				strcpy(year, ptr);
				ptr = strtok(NULL, "-");
				strcpy(month, ptr);
				//txt file format
				/*
				Customer	Address	Amount
				Kim Yongdam	AS816	$3000
				------------------------------
				Itemized Billing List
				====================================================================================================
				| Package Number | Item | Amount | Serivce Type | Payment Type | Timeliness of Delivery|
				====================================================================================================
				P00000011 | airPod Pro | $350 | Flat Envelope | Credit Card | Overnight
				P03002111 | iPhone 11 Pro, MG, 256GB | $1400 | Small Box | Prepaid(reserved) | Second day
				P10222935 | Macbook Air, RG, i5/16GB/512GB | $0 | Large Box | Returned(malfunctioning) | Returned
				P10222935 | Macbook Air, RG, i5/16GB/512GB | $1250 | Large Box | 5 days(delayed)
				----------------------------------------------------------------------------------------------------

				*/

				//make output file name
				strcat(output_filename, "_");
				strcat(output_filename, year);
				strcat(output_filename, month);
				strcat(output_filename, "_");
				strcat(output_filename, name);
				strcat(output_filename, ".txt");

				//remake ym
				strcat(ym, "-");
				strcat(ym, month);
				
				//write bill on setting output file name
				FILE * o_fp = fopen(output_filename, "w");
				fprintf(o_fp, "Customer Address\tAmount\n");
				
				//------------------------------------------------query
				char query1[1024] = "with cost_sum(customer_id, ym, sum_cost) as (select customer_id, date_format(yearmonth, \'%Y-%m\'), sum(cost) from payment group by customer_id, date_format(yearmonth, \'%Y-%m\')) select customer_id, name, location, sum_cost from customer join cost_sum using(customer_id) where ym = \'";
				strcat(query1, ym);
				strcat(query1, "\' and customer.name = \'");
				strcat(query1, name);
				strcat(query1, "\'");
				int state = 0;
				
				state = mysql_query(connection, query1);
				if (state != 0)
				{
					printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
					return 1;
				}
				
				
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						fprintf(o_fp, "%s\t%s\t%s\n", name, sql_row[2], sql_row[3]);
					}
					printf("\n");
					mysql_free_result(sql_result);
				}


				fprintf(o_fp, "------------------------------\n");
				fprintf(o_fp, "Itemized Billing List\n");
				fprintf(o_fp, "====================================================================================================\n");
				fprintf(o_fp, "| Package Number | Item | Amount | Serivce Type | Prepaid(Yes/No) | Timeliness of Delivery|\n");
				fprintf(o_fp, "====================================================================================================\n");
				//------------------------------------------------query
				char query2[256] = "select package_id, content, cost, type_of_package, prepaid, timeliness from((package join payment using(package_id)) join service using(package_id)) join customer using(customer_id) where date_format(yearmonth, \'%Y-%m\') = \'";
				strcat(query2, ym);
				strcat(query2, "\' and name = \'");
				strcat(query2, name);
				strcat(query2, "\'");
				state = 0;

				state = mysql_query(connection, query2);
				if (state != 0)
				{
					printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
					return 1;
				}
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						fprintf(o_fp, "%s\t|%s\t|%s\t|%s\t|%s\t|%s\t\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5]);
					}
					printf("\n");
					mysql_free_result(sql_result);
				}
				fclose(o_fp);
				
				printf("Generation Completed\n");
			}
		}
	}
}

//function for create table and insert data
int pre_sql(MYSQL* connection, MYSQL* conn_) {

	char query[1023];
	int state = 0;

	FILE* fp = fopen("20141362_1.txt", "r");//txtfile for  reate table and insert data
	if (!fp) {
		printf("No such file!\n");
		return -1;
	}
	while (!feof(fp)) {
		fgets(query, sizeof(query), fp);
		query[strlen(query) - 1] = '\0';
		//printf("%s\n",query);
		state = mysql_query(connection, query);
		if (state != 0)
		{
			printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
			return -1;
		}
	}
	fclose(fp);
	return 0;
}
//functino for delete and drop table
int del_sql(MYSQL* connection, MYSQL* conn_) {
	char query[1023];
	int state = 0;

	FILE* fp = fopen("20141362_2.txt", "r");//txtfile for  reate table and insert data
	if (!fp) {
		printf("No such file!\n");
		return -1;
	}
	while (!feof(fp)) {
		fgets(query, sizeof(query), fp);
		query[strlen(query) - 1] = '\0';
		//printf("%s\n",query);
		state = mysql_query(connection, query);
		if (state != 0)
		{
			printf("%d ERROR : %s\n", mysql_errno(conn_), mysql_error(conn_));
			return -1;
		}
	}
	fclose(fp);
	return 0;
}

