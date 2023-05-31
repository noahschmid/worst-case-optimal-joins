import pandas as pd
from pandasql import sqldf
pysqldf = lambda q: sqldf(q, globals())
separator = ' '
table1 = pd.read_csv('./mini_mini_data_1.tsv', sep=separator)
table2 = pd.read_csv('./mini_mini_data_2.tsv', sep=separator)
table3 = pd.read_csv('./mini_mini_data_3.tsv', sep=separator)


result = pysqldf('''
    SELECT table1.v1 as v1, table2.v2 as v2, table3.v3 as v3
    FROM table1, table2, table3
    WHERE table1.v2 = table2.v2
    AND table2.v3 = table3.v3
    AND table1.v1 = table3.v1
    ''')


# print number of rows
print(result.shape[0])
result.to_csv('./mini_mini_expected_result.tsv', sep=separator, index=False)
