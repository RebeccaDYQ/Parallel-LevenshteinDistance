#!/usr/bin/python

class EditDistance():
	
	def __init__(self):
		"""
		Do not change this
		"""
	
	def calculateLevenshteinDistance(self, str1, str2):
		"""
		TODO:
			take two strings and calculate their Levenshtein Distance for task 1 
			return an integer which is the distance
		"""
		len_1 = len(str1) + 1
		len_2 = len(str2) + 1
		D = [[0 for x in range(len_2)] for y in range(len_1)]

		for i in range(len_1):
			D[i][0] = i
		for j in range(len_2):
			D[0][j] = j

		for i in range(1, len_1):
			for j in range(1, len_2):
				
				add_one = 1
				if str1[i-1] == str2[j-1]:
					add_one = 0
				D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one)

		# print(D)
		return D[len_1-1][len_2-1]
		
	def calculateOSADistance(self, str1, str2):
		"""
		TODO:
			take two strings and calculate their OSA Distance for task 2 
			return an integer which is the distance
		"""
		len_1 = len(str1) + 1
		len_2 = len(str2) + 1
		D = [[0 for x in range(len_2)] for y in range(len_1)]

		for i in range(len_1):
			D[i][0] = i
		for j in range(len_2):
			D[0][j] = j

		for i in range(1, len_1):
			for j in range(1, len_2):	
				# substitution
				add_one = 1
				if str1[i-1] == str2[j-1]:
					add_one = 0
				D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one)
				# transposition
				if i > 1 and j > 1 and str1[i-1] == str2[j-2] and str1[i-2] ==str2[j-1]:
				    D[i][j] = min(D[i][j], D[i-2][j-2] + 1)
				
		# print(D)
		return D[len_1-1][len_2-1]

		
	def calculateDLDistance(self, str1, str2):
		"""
		TODO:
			take two strings and calculate their DL Distance for task 3 
			return an integer which is the distance
		"""

		# new array of |Σ| integers
		da = [0 for x in range(26)]
	    
		len_1 = len(str1)
		len_2 = len(str2)
		d = [[0 for x in range(len_2+2)] for y in range(len_1+2)]

		maxdist = len_1 + len_2
		d[0][0] = maxdist

		for i in range(1, len_1+2):
			d[i][0] = maxdist
			d[i][1] = i-1

		for j in range(1, len_2+2):
			d[0][j] = maxdist
			d[1][j] = j-1

		for i in range(2, len_1+2):
			db = 0
			for j in range(2, len_2+2):
				k = da[ord(str2[j-2]) - ord('a')]
				l = db
				cost = 1
				if str1[i-2] == str2[j-2]:
					cost = 0
					db = j
				d[i][j] = min(d[i-1][j-1] + cost, d[i][j-1] + 1)
				d[i][j] = min(d[i][j], d[i-1][j] + 1)
				d[i][j] = min(d[i][j], d[k-1][l-1] + (i-k-1) + 1 + (j-l-1))                                                                
			da[ord(str1[i-2]) - ord('a')] = i

		print(d)
		return d[len_1+1][len_2+1]