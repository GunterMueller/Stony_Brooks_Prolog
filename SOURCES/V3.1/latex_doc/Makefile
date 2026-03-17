
all: sbprolog_doc.ind sbprolog_doc.pnd sbprolog_doc.dvi

sbprolog_doc.dvi sbprolog_doc.idx sbprolog_doc.pdx : sbprolog_doc.tex
	latex sbprolog_doc.tex
	latexpdf sbprolog_doc.tex

sbprolog_doc.ind : sbprolog_doc.idx
	makeindex sbprolog_doc.idx

sbprolog_doc.pnd : sbprolog_doc.pdx
	makeindex -s predindex.idxsty -o sbprolog_doc.pnd -t sbprolog_doc.plg sbprolog_doc.pdx

clean:
	/bin/rm -f *.aux *.log *.dvi *.bak
