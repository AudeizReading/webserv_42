#!/usr/bin/perl 

# --- OU LES CHOSES SERIEUSES DOIVENT SE PASSER --------------------------------
if ($ENV{'REQUEST_METHOD'} eq "POST" ) 
{
	# l'upload doit se passer dans cette partie, on read les donnees puis on les envoie vers un fichier
	# check for upload file
	#  $buffer = $ENV{'QUERY_STRING'};
	%_GET = &split_cgi_array($ENV{'QUERY_STRING'});
	if ($ENV{'CONTENT_LENGTH'} > 0)
	{
		print "We are waiting for ".$ENV{'CONTENT_LENGTH'}." octets.\r\n";
		print "We are waiting for ".$ENV{'CONTENT_TYPE'}.".\r\n";

		read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
		$output_mess="STDIN (Methode POST)" ;
		%_POST=&split_cgi_array($buffer);
	}

	&cgi_response_header(200, "OK");
	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	&cgi_print_html_body_begin();
	&cgi_print_html_double_elt("h1", "Résultat de la requete POST");
	&cgi_print_html_double_elt("h2", $output_mess);
	&cgi_print_html_double_elt("p", "Raw Datas:</br> <b>$buffer</b>");
	&cgi_print_html_double_elt("h2", "Liste des informations décodées");
	print STDOUT "\t<ul>\r\n";
	&cgi_print_array_html(%_GET);
	&cgi_print_array_html(%_POST);
	&cgi_print_array_html(%ENV);
	print STDOUT "\t</ul>\r\n";
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
elsif ($ENV{'REQUEST_METHOD'} eq "DELETE")
{
	# TODO: delete content
	&cgi_response_header(200, "OK");
	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	&cgi_print_html_body_begin();
	&cgi_print_html_double_elt("h1", "Résultat de la requete DELETE");
	&cgi_print_html_double_elt("h2", $output_mess);
	&cgi_print_html_double_elt("p", "Raw Datas:</br> <b>$buffer</b>");
	&cgi_print_html_double_elt("h2", "Liste des informations décodées");
	&cgi_print_html_double_elt("p", "Not handled Yet");
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
elsif ($ENV{'REQUEST_METHOD'} eq "GET")
{
	# il faut aussi recup PATH_INFO dans le cas d'une req GET
    $output_mess="QUERY_STRING (Methode GET)";
    $buffer = $ENV{'QUERY_STRING'};
	%_GET = &split_cgi_array($ENV{'QUERY_STRING'});

	&cgi_response_header(200, "OK");
	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	&cgi_print_html_body_begin();
	&cgi_print_html_double_elt("h1", "Résultat de la requete GET");
	&cgi_print_html_double_elt("h2", $output_mess);
	&cgi_print_html_double_elt("p", "Raw Datas:</br> <b>$buffer</b>");
	&cgi_print_html_double_elt("h2", "Liste des informations décodées");
	print STDOUT "\t<ul>\r\n";
	&cgi_print_array_html(%_GET);
	print STDOUT "\t</ul>\r\n";
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
else # other methods that we do not handle
{
	&cgi_response_header(403, "Forbidden");
	print STDOUT "Hello ".@_;
	exit 1;
}
# --- OU LES CHOSES SERIEUSES DOIVENT FINIR DE SE PASSER -----------------------

# split by & and =, put this in a sort of map
# works with GET and also POST and ENV
sub split_cgi_array
{
	local (@pairs, $pair, $name, $value, %form, $arg);

	($arg)=@_;
	@pairs = split(/&/, $arg);

    foreach $pair (@pairs) 
    {
        ($name, $value) = split(/=/, $pair);
        $value =~ tr/+/ /;
        $value =~ s/%(..)/pack("C", hex($1))/eg;
        $form{$name} = $value;
	}
	%form;
}

# print any array nom
sub cgi_print_array_html
{
	local (%array);

	%array = @_;
	foreach $match (keys (%array)) {
		print STDOUT "\t\t<li><b>$match: </b>".$array{$match}."</li>\n";
	}
}

sub cgi_print_html_dtd
{
	print STDOUT "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n";
}

sub cgi_print_html_begin
{
	print STDOUT "<html lang=\"fr\">\r\n";
}

sub cgi_print_html_head
{
	print STDOUT "<head>\r\n";
	print STDOUT "\t<meta charset=\"UTF-8\">\r\n";
	print STDOUT "\t<title>$ENV{'REQUEST_METHOD'}</title>\r\n";
	print STDOUT "</head>\r\n";
}

sub cgi_print_html_body_begin
{
	print STDOUT "<body bgcolor=\"#FFBFFF\">\r\n";
}

sub cgi_print_html_end
{
	print STDOUT "</html>\r\n";
}

sub cgi_print_html_body_end
{
	print STDOUT "</body>\r\n";
}

sub cgi_print_html_double_elt
{
	local ($elt, $value)=@_;
	print STDOUT "<".$elt.">".$value."</".$elt.">\r\n";
}

sub	cgi_response_header
{
	local ($code_response, $message_response)=@_;

	# Use a partial custom header functionnality
	print $ENV{'SERVER_PROTOCOL'}." ".$code_response." ".$message_response."\r\n";
	print "Content-Type: text/html\r\n";
	print "\r\n";



	#print $ENV{'SERVER_PROTOCOL'}." 200 OK\r\n"; # voir par la suite pour passer la rep en arg de la fn
	# la date ca peut fonctionner comme ca: soit on `date` et on formatte la sortie de la cmd
	#print "Date: ".`date`."\r\n"; # Pas le bon format
	# soit on utilise la facon perl (perldoc perfunc -> chercher localtime)
	# on est dans un contexte de liste:
	# ($sec, $min, $hour, $day_of_month, $month, $years_since_1900, $day_of_week, $day_of_year, $is_winter_time)=localtime;
	#print $day_of_month." / ".$month; # par exemple
	#print "Server: ".$ENV{'SERVER_SOFTWARE'}."\r\n";
	#print "Cache-Control: no-cache\r\n";
	#print "Content_length : ".$ENV{'CONTENT_LENGTH'}."\n"; # Pas obligatoire, vaut mieux rien mettre
}

