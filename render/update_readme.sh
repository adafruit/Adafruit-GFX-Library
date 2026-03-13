sed -i -n '0,/## Fonts/p' README.md
for FILE in *.png; do
	NAME=${FILE%.*};
	echo -e "### $NAME\n![$NAME]($FILE)\n";
done >> README.md
