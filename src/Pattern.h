#pragma once

struct PatternRow;

class Pattern
{
	PatternRow *rows;
	
public:	
	static const int maxRows = 256;
	
	PatternRow& getRow(int row);
	int getLastUsedRow() const;
	void clear();
	int getLastMacroUsed() const;
	bool isEmpty() const;	
	
	void insertRow(int row, int flags);
	void deleteRow(int row, int flags);

	Pattern();
	virtual ~Pattern();
};
