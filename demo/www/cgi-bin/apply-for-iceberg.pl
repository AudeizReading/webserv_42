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

	%_GET = &split_cgi_array($ENV{'QUERY_STRING'});
	if ($ENV{'CONTENT_LENGTH'} > 0)
	{
		print "We are waiting for ".$ENV{'CONTENT_LENGTH'}." octets.\r\n";
		print "We are waiting for ".$ENV{'CONTENT_TYPE'}.".\r\n";

		read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
		%_POST=&split_cgi_array($buffer);
	}
	if (defined($ENV{'PATH_INFO'}) || defined($_GET{'path_info'})) # Is there a path_info where searching datas ?
	{
		#$directory = $ENV{'PATH_TRANSLATED'};
		$directory = "..".$ENV{'PATH_INFO'};
		if (defined($_GET{'path_info'}))
		{
			# ./demo/www//upload -> resultat obtenu, et suffisant pour opendir
			#$directory = $ENV{'PATH_TRANSLATED'}.$_GET{'path_info'};
			# avec le chdir du CGIManager, il faut aussi changer le path d'acces, nous sommes dans le rep cgi-bin
			$directory = "..".$_GET{'path_info'};
		}
	}
	$absolute_path = $ENV{'DOCUMENT_ROOT'}.$ENV{'PATH_INFO'};

	&cgi_print_html_double_elt("p", "directory: $directory");
	&cgi_print_html_double_elt("p", "absolute path: $absolute_path");
	&cgi_print_html_double_elt("p", "ENV{'PATH_TRANSLATED'}: $ENV{'PATH_TRANSLATED'}");
	&cgi_print_html_double_elt("p", "_GET{'path_info'}: $_GET{'path_info'}");
	&cgi_print_html_double_elt("p", "_POST{'path_info'}: $_POST{'path_info'}");
	&cgi_print_html_double_elt("p", "ENV{'PATH_INFO'}: $ENV{'PATH_INFO'}");

	#opendir(DIRECTORY_FD, $directory) || die "$directory couldn't be opened: $!";
	#	@FILES = grep(/\.png|jpe?g$/i, readdir DIRECTORY_FD);
	if (defined($_POST{'filename'}))
	{
		$_FILES{'filename'} = $_POST{'filename'};
		&cgi_print_html_double_elt("p", "filename: ".$_FILES{'filename'});
		$num=0;
		&cgi_print_html_double_elt("p", "filename".$num.": ".$_FILES{'filename'});
	}
	else
	{
		if (defined($_POST{'nb_files'}))
		{
			foreach $file (values (%_POST))
			{
				&cgi_print_html_double_elt("p", "filename (avant de check le modele multi):".$file);
				if ($_POST{$file} =~ m/jp?g|png$/)
				{
					&cgi_print_html_double_elt("p", "filename (multi):".$file);
				}
			}
		}
	}
	print STDOUT "\t<ul>\r\n";
	&cgi_print_html_double_elt("h1", "Résultat de la requete DELETE");
	&cgi_print_html_double_elt("p", "Raw Datas:</br> <b>$buffer</b>");
	&cgi_print_html_double_elt("h2", "Liste des informations décodées");
	#	&cgi_debug(0, %_GET);
	&cgi_debug(0, %_POST);
	# recuperer le nom du fichier a delete
	# ouvrir le rep upload
	# chercher le fichier
	# delete le fichier
	# afficher le resultat de l'operation -> le snippet javascript sur l'output peut le gerer plus facilement
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
elsif ($ENV{'REQUEST_METHOD'} eq "GET")
{
    $buffer = $ENV{'QUERY_STRING'};
	%_GET = &split_cgi_array($ENV{'QUERY_STRING'});

	&cgi_response_header(200, "OK");
	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	&cgi_print_html_body_begin();
	&cgi_print_html_double_elt("h1", "Notre collection d'icebergs");

	#&cgi_debug(0, %_GET);
	if (defined($ENV{'PATH_INFO'}) || defined($_GET{'path_info'})) # Is there a path_info where searching datas ?
	{
		$directory = "../".$ENV{'PATH_INFO'};
		#$directory = $ENV{'PATH_TRANSLATED'};
		if (defined($_GET{'path_info'}))
		{
			# ./demo/www//upload -> resultat obtenu, et suffisant pour opendir
			#$directory = $ENV{'PATH_TRANSLATED'}.$_GET{'path_info'};
			# avec le chdir du CGIManager, il faut aussi changer le path d'acces, nous sommes dans le rep cgi-bin
			$directory = "../".$_GET{'path_info'};
		}
		
		$absolute_path = $ENV{'PATH_TRANSLATED'}."cgi-bin/".$_GET{'path_info'};
		opendir(DIRECTORY_FD, $directory) || die "$directory couldn't be opened: $!";
		@FILES = grep(/\.png|jpe?g$/i, readdir DIRECTORY_FD);
		print STDOUT "\t<ul>\r\n";
		foreach $file (@FILES)
		{
			# ici mettre path /upload/$file pour que ca route derriere le cgi
			# faire un systeme de pagination si trop de photos
			print "<img src=\"$_GET{'path_info'}/$file\"/>";

			# proposition for deleting this file
			print "<form id=\"delete-form\" method=\"DELETE\" action=\"/cgi-bin/apply-for-iceberg.pl?/upload\" enctype=\"multipart/form-data\" alt=\"Deletion files(s)\">";
			print "<p>";
			print "<input type=\"hidden\" name=\"path_info\" filename=\"$_GET{'path_info'}/$file\" value=\"/upload\"/>";
			print "</p>";
			print "<p>";
			print "<input type=\"submit\" name=\"submit\" value=\"Supprimer un iceberg\" />";
			print "<input type=\"reset\" name=\"reset\" value=\"Reset\" />";
			print "</p>";
			print "<pre id=\"output\" style=\"max-height:300px;\">";
			print "</pre>";
			print "</form>";

			print "<script>";
			print "document.getElementById('delete-form').addEventListener('submit', function (event) {\r\n";
			print "	const data = new URLSearchParams(new FormData(document.getElementById('delete-form')));\r\n";
			#
			print "data.append(\"filename\", \"$_GET{'path_info'}/$file\")\r\n";
			print "let nb_files = 1;\r\n";
			print "data.append(\"nb_files: \", nb_files.toString());\r\n";

			print "fetch(document.getElementById('delete-form').action, {\r\n";
			print "	method: 'DELETE',\r\n";
			print "	body: data,\r\n";
			print "	}).then(function(response) {\r\n";
			print "		return response.text()\r\n";
			print "	})\r\n";
			print "	.then((data) => {\r\n";
			print "		document.getElementById('output').textContent = \"Request sent\";\r\n";
			#print "		document.getElementById('output').textContent = data;\r\n";
			print "	})\r\n";
			print "		event.preventDefault()\r\n";
			print 	"event.preventDefault()";
			print 	"})\r\n";
			print "	</script>\r\n";
		}
		print STDOUT "\t</ul>\r\n";
		closedir(DIRECTORY_FD);
	}
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
else # other methods that we do not handle
{
	&cgi_response_header(403, "Forbidden");
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
	local (%array) = @_;

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
	print STDOUT "<body bgcolor=\"lightblue\">\r\n";
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

sub	cgi_debug
{
	local ($need_env, %array) = @_;
	print STDOUT "<p>DEBUG<br/>\t<ul>\r\n";
	&cgi_print_array_html(%array);
	if ($need_env > 0)
	{
		&cgi_print_array_html(%ENV);
	}
	print STDOUT "\t</ul></p>\r\n";
}
