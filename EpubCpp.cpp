#include <iostream>
#include <regex>
#include <map>
extern "C" {
#include <archive.h>
#include <archive_entry.h>
#include <libxml/list.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
};

using namespace std;

struct epub_handle {
	epub_handle (string filename)
	{
		init (filename);
	}
	~epub_handle ()
	{
		archive_read_free (ar);
	}


	void init (string& fn)
	{
		int r(0);
		ar=archive_read_new ();
		ent=archive_entry_new ();

		if (!ar)
			throw 1;

		try{
			r = archive_read_support_filter_all (ar);

			if (r)
				throw r;

			r = archive_read_support_format_all (ar);

			if (r)
				throw r;

			r = archive_read_open_filename (ar, fn.c_str (), 1054);

			if (r)
			{
				throw r;
			}
		}catch(int& error)
		{
			delete this;
			throw;
		}
	}
	struct archive_entry*
		next (map<string, string>& cont_b)
	{
		int r(archive_read_next_header (ar, &ent));
		if (r) throw r;
		string cont="";
		char buf[1054];

		while (1)
		{
			r=archive_read_data (ar, buf, 1054);
			if (!r) break;
			cont=cont+string (buf, r);
		}
		cont_b[string (archive_entry_pathname (ent))]=cont;
		return ent;
	}

	struct archive *ar;
	struct archive_entry *ent;
};

char *findd (xmlNodePtr p, char *name)
{
		while (p)
		{
				if (!strcmp ((char*)p->name, name))
						return(char*) p->children->content;
				p=p->next;
		}
		return (0);
}

void ShoP (xmlNodePtr p, map<string, string>& mm)
{
		if (!p) return;

		p = p->children;
		regex htmlrx("^.*\\.x?html$");

		while (p)
		{
				if (p->name && !strcmp ("metadata",(char*) p->name))
				{
				xmlNodePtr xp = p->children;
				}else
				if (p->name && !strcmp ("manifest", (char*)p->name))
				{
					xmlNodePtr xp = p->children;

					while (xp)
					{
							char *txt=findd ((xmlNodePtr)xp->properties, "href");
							xp=xp->next;
							if (!txt)
								continue;
							do {
								if (!regex_match (string (txt),
											htmlrx))
									continue;
								for (auto& cont_: mm)
								{
									const char *xnn=
									cont_.first.c_str ();
									if (strlen (txt)>=strlen (xnn))
										continue;
									if (string (&xnn[strlen(xnn)-strlen(txt)])
									==string (txt))
									{
										cout << cont_.first << endl;
									}
								}
							} while (0);
					}
				}
				p=p->next;
		}
}
int main (int argsc, char **args)
{
	epub_handle* ep=new epub_handle(string (args[1]));
	map<string, string> cont_b;
	while (1)
	{
		try {
			ep->next (cont_b);
		} catch (int& er)
		{
			break;
		}
	}
	regex opf("^.*\\.o..$");
	for (auto& cont_: cont_b)
	{
		if (!regex_match (cont_.first, opf))
			continue;
		xmlDocPtr dp= xmlReadMemory (cont_.second.c_str(),
				cont_.second.length(),
				"utf-8", 0, 0);

		do {
				if (!dp) break;
				ShoP (dp->children, cont_b);
				xmlFreeDoc (dp);
		} while (0);
		break;
	}
	return( 0 );
}
