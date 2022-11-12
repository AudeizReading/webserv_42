#!/usr/bin/perl 

use POSIX;
# --- OU LES CHOSES SERIEUSES DOIVENT SE PASSER --------------------------------
if ($ENV{'REQUEST_METHOD'} eq "POST" ) 
{

	# l'upload doit se passer dans cette partie, on read les donnees puis on les envoie vers un fichier
	# check for upload file
	%_GET = &cgi_parse_request_string($ENV{'QUERY_STRING'});
	if ($ENV{'CONTENT_LENGTH'} > 0)
	{
		print "We are waiting for ".$ENV{'CONTENT_LENGTH'}." octets.\r\n";
		print "We are waiting for ".$ENV{'CONTENT_TYPE'}.".\r\n";

		binmode STDIN;

		$len_read = read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
		$len = length $buffer;
		print "<p> buffer lenght: $len</p>";
		print "<p> read lenght: $len_read</p>";

		# It seems that we do not read the same size of datas as expected, do not know why but we receive less than content length, so we do not have the full datas and the upload "failed" -> not really failed, but as we do not have the full datas, the file is created but the datas are corrupted (the new upload file can be seen in the gallery but as broken link, try to upload one file, you will be able to delete it from the gallery)
		if (($len == $len_read && $len_read == $ENV{'CONTENT_LENGTH'}) || warn "We've read $len_read bytes but we are expecting $ENV{'CONTENT_LENGTH'}.")
		{
		}
		if (defined($ARGV[0]) || warn "It misses the boundary keys!") # Means that a boundary key is passed to the cgi script
		{
			&cgi_print_html_double_elt("p", "Another upload? <a href=\"../upload.html\">Click Here:</a>");
			$boundary = $ARGV[0];
			#&cgi_simulate_body_upload($boundary);
			if (($buffer =~ $boundary && $buffer =~ /(Content-Type\:\ image\/png|jpeg|jpg\ \n)/) || die "These datas are not allowed to be host on the server.")
			{
					@post_datas = split($boundary, $buffer);
					@upload_datas = split(/\n\r\n/, $post_datas[1]);
					@form_post_datas = split(/[=;:"' ]/, $upload_datas[0]);
						#foreach $data (@post_datas)
						#{
						#	$len = length $data;
						#	&cgi_print_html_double_elt("pre", "post_datas ==> $data");
						#}
					$upload_filename = $form_post_datas[11];

						foreach $data (@upload_datas)
						{
							&cgi_print_html_double_elt("pre", "Hi <-> upload_datas ==> $data");
							$len = length $data;
							print "<p> lenght: $len</br>";
							for(my $i=0; $i < $len; ++$i) {
								my $ascii = substr($data, $i, 1);
								$ascii = ord($ascii);
								&cgi_print_html_double_elt("span", "==> $ascii <==</br>");
							}
							print "</p>";
						}
						#foreach $data (@form_post_datas)
						#{
						#	$len = length $data;
						#	print "<p> lenght: $len</p>";
						#	&cgi_print_html_double_elt("pre", "form_post_datas ==> $data");
						#}
					&cgi_print_html_double_elt("p", "upload filename: $upload_filename\n\r");

					open(UPLOAD_FILE, ">:raw", "../upload/$upload_filename") || die "$upload_filename couldn't be opened: $!";
					binmode UPLOAD_FILE;
					print UPLOAD_FILE $upload_datas[1] || die "$upload_filename couldn't be written: $!";
					close UPLOAD_FILE;
			}
		}
		$output_mess="STDIN (Methode POST)" ;
	}

	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	#&cgi_print_html_body_begin();
	#&cgi_print_html_double_elt("h1", "Résultat de la requete POST");
	#&cgi_print_html_double_elt("h2", $output_mess);
	#&cgi_print_html_double_elt("div", "<p>Raw Datas:</p> <pre>$buffer</pre>");
	#&cgi_print_html_double_elt("h2", "Liste des informations décodées");
	#print STDOUT "\t<ul>\r\n";
	#print "ARGV[0]\r\n";
	#&cgi_print_html_double_elt("li", $ARGV[0]);
	#print "GET\r\n";
	#&cgi_print_array_html(%_GET);
	#print "ENV\r\n";
	#&cgi_print_array_html(%ENV);
	#print STDOUT "\t</ul>\r\n";
	&cgi_print_html_double_elt("p", "Another upload? <a href=\"../upload.html\">Click Here:</a>");
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
elsif ($ENV{'REQUEST_METHOD'} eq "DELETE")
{
	%_GET = &cgi_parse_request_string($ENV{'QUERY_STRING'});
	if ($ENV{'CONTENT_LENGTH'} > 0)
	{
		read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
		%_POST=&cgi_parse_request_string($buffer);
	}
	if (defined($ENV{'PATH_INFO'}) || defined($_GET{'path_info'})) # Is there a path_info where searching datas ?
	{
		$directory = "..".$ENV{'PATH_INFO'};
		if (defined($_GET{'path_info'}))
		{
			$directory = "..".$_GET{'path_info'};
		}
		elsif (defined($_POST{'path_info'}))
		{
			$directory = "..".$_POST{'path_info'};
		}
	}

	# recuperer le nom du fichier a delete
	if (defined($_POST{'filename'})) # only 1 file
	{
		$file = $_POST{'filename'};
		#delete here
		if ( -e "../$file" and -f "../$file")
		{
			#delete here
			unlink("../$file") || warn "../$file couldn't be deleted: $!";
			&cgi_print_html_double_elt("p", "The file ../$file has been deleted"); 
		}
		else
		{
			die "You are trying to delete an element that is not on the server or that you are not allowed to access!";
		}
	}
	else # multi file
	{
		if (defined($_POST{'nb_files'}))
		{
			@_keys = grep(/filename/, (keys (%_POST)));
			foreach $file (@_keys)
			{
				if ( -e "$directory/$_POST{$file}" and -f "$directory/$_POST{$file}")
				{
					#delete here
					unlink("$directory/$_POST{$file}");
					&cgi_print_html_double_elt("p", "The file $directory/$_POST{$file} has been deleted"); 
				}
				else
				{
					die "You are trying to delete an element that is not on the server or that you are not allowed to access!";
				}
			}
		}
	}
}
elsif ($ENV{'REQUEST_METHOD'} eq "GET")
{
    $buffer = $ENV{'QUERY_STRING'};
	%_GET = &cgi_parse_request_string($ENV{'QUERY_STRING'});

	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	&cgi_print_html_body_begin();
	&cgi_print_html_double_elt("h1", "Our icebergs' collection");

	if (defined($ENV{'PATH_INFO'}) || defined($_GET{'path_info'})) # Is there a path_info where searching datas ?
	{
		$directory = "../".$ENV{'PATH_INFO'};
		#$directory = $ENV{'PATH_TRANSLATED'};
		if (defined($_GET{'path_info'}))
		{
			# ./demo/www/upload -> resultat obtenu, et suffisant pour opendir
			#$directory = $ENV{'PATH_TRANSLATED'}.$_GET{'path_info'};
			# avec le chdir du CGIManager, il faut aussi changer le path d'acces, nous sommes dans le rep cgi-bin
			$directory = "../".$_GET{'path_info'};
		}
		
		$absolute_path = $ENV{'PATH_TRANSLATED'}."cgi-bin/".$_GET{'path_info'};
		opendir(DIRECTORY_FD, $directory) || die "$directory couldn't be opened: $!";
		@FILES = grep(/\.png|jpe?g$/i, readdir DIRECTORY_FD);

		&cgi_display_files($_GET{'path_info'}, @FILES);
		closedir(DIRECTORY_FD);
	}
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_double_elt("p", "Book another iceberg? <a href=\"../upload.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
else # other methods that we do not handle
{
	exit 1;
}
# --- OU LES CHOSES SERIEUSES DOIVENT FINIR DE SE PASSER -----------------------

sub	cgi_simulate_body_upload
{
	my $arg = @_;
	print STDOUT "<p>arg: $arg</p>";
}
# split by & and =, put this in a sort of map
# works with GET and also POST and ENV
sub cgi_parse_request_string
{
	my (@pairs, $pair, $name, $value, %form, $arg);

	($arg) = @_;
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
	print STDOUT "<link href=\"../main-style.css\" rel=\"stylesheet\" type=\"text/css\">"; # maybe the link could vary
	print STDOUT "<link href=\"http://fonts.cdnfonts.com/css/iceberg\" rel=\"stylesheet\">";
	print STDOUT "<script src=\"https://kit.fontawesome.com/dedab4d2ca.js\" crossorigin=\"anonymous\"></script>";
	print STDOUT "</head>\r\n";
}

sub cgi_print_html_body_begin
{
	print STDOUT "<body>\r\n";
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
	my ($elt, $value) = @_;

	print STDOUT "<".$elt.">".$value."</".$elt.">\r\n";
}

sub cgi_print_html_input_submit_reset
{
	my ($value) = @_;

	print "<p>";
	print "<input type=\"submit\" name=\"submit\" value=\"$value\" />";
	print "<input type=\"reset\" name=\"reset\" value=\"Reset\" />";
	print "</p>";
}

sub	cgi_response_header
{
	my ($code_response, $message_response) = @_;

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
	my ($need_env, %array) = @_;

	print STDOUT "<p>DEBUG<br/>\t<ul>\r\n";
	&cgi_print_array_html(%array);
	if ($need_env > 0)
	{
		&cgi_print_array_html(%ENV);
	}
	print STDOUT "\t</ul></p>\r\n";
}

sub cgi_delete_request_javascript
{
	my ($i, $path_info, $file) = @_;

	print "<script>";
	print "document.getElementById('delete-form$i').addEventListener('submit', function (event) {\r\n";
	print "	const data = new URLSearchParams(new FormData(document.getElementById('delete-form$i')));\r\n";
	#
	print "data.append(\"filename\", \"$path_info/$file\")\r\n";
	print "let nb_files = 1;\r\n";
	print "data.append(\"nb_files\", nb_files.toString());\r\n";

	print "fetch(document.getElementById('delete-form$i').action, {\r\n";
	print "	method: 'DELETE',\r\n";
	print "	body: data,\r\n";
	print "	}).then(function(response) {\r\n";
	print "		return response.text()\r\n";
	print "	})\r\n";
	print "	.then((data) => {\r\n";
	print "		document.getElementById('output').textContent = \"Request sent\";\r\n";
	print "location.reload();\r\n";
	#print "		document.getElementById('output').textContent = data;\r\n";
	print "	})\r\n";
	print "		event.preventDefault()\r\n";
	print 	"event.preventDefault()";
	print 	"})\r\n";
	print "	</script>\r\n";

}

sub cgi_display_files
{
	my ($path_info, @files) = @_;
	my $i = 0;

	print STDOUT "\t<ul class=\"gallery\">\r\n";
	foreach $file (@files)
	{
		# ici mettre path /upload/$file pour que ca route derriere le cgi
		# faire un systeme de pagination si trop de photos
		&cgi_print_html_double_elt("li", "<img class=\"img_gallery\" src=\"$path_info/$file\"/>"); 

		print "<form class=\"delete-form\" id=\"delete-form$i\" method=\"DELETE\" action=\"/cgi-bin/apply-for-iceberg.pl?/upload\" enctype=\"multipart/form-data\" alt=\"Deletion files(s)\">";
		&cgi_print_html_double_elt("p", "<input type=\"hidden\" name=\"path_info\" filename=\"$path_info/$file\" value=\"/upload\"/>"); 
		&cgi_print_html_input_submit_reset("Delete an Iceberg");

		# where the javascript sinppet displays its output
		print "<pre id=\"output\" style=\"max-height:300px;\">";
		print "</pre>";
		print "</form>";

		&cgi_delete_request_javascript($i, $path_info, $file);
		++$i;
	}
	print STDOUT "\t</ul>\r\n";
}
