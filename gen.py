import logging
import os
import time
import shutil
import re

import jinja2
import click
import yaml
import markdown
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

import config


_logger = logging.getLogger(__name__)


def _render_to_file(filepath, template, ctx):
    fp = os.path.join(config.BUILD_ROOT, filepath)
    with open(fp, "wt") as f:
        f.write(template.render(**ctx))


def _find_md_links(md):
    """
    Return dict of links in markdown
    From: https://stackoverflow.com/questions/30734682/extracting-url-and-anchor-text-from-markdown-using-python
    """
    INLINE_LINK_RE = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
    FOOTNOTE_LINK_TEXT_RE = re.compile(r'\[([^\]]+)\]\[(\d+)\]')
    FOOTNOTE_LINK_URL_RE = re.compile(r'\[(\d+)\]:\s+(\S+)')

    links = dict(INLINE_LINK_RE.findall(md))
    footnote_links = dict(FOOTNOTE_LINK_TEXT_RE.findall(md))
    footnote_urls = dict(FOOTNOTE_LINK_URL_RE.findall(md))

    for key, value in footnote_links.items():
        footnote_links[key] = footnote_urls[value]
    links.update(footnote_links)

    return links


def _load_data(no_wip):
    fp = os.path.join(config.SRC_ROOT, config.DATA_FILE)

    with open(fp) as stream:
        data = yaml.safe_load(stream)

    refs = {}

    for m in data["machines"]:
        m["url_id"] = m["name"].strip().replace(" ", "_").lower()

    if no_wip:
        data["questions"] = list(filter(lambda x: "wip" not in x or x["wip"] == False, data["questions"]))

    for q in data["questions"]:
        q["url_id"] = q["title"].strip().replace(" ", "_").lower()
        if "machine" in q:
            q["machine_url_id"] = q["machine"].strip().replace(" ", "_").lower()

    num_q = len(data["questions"])
    for i in range(num_q):
        data["questions"][i]["next_url_id"] = data["questions"][(i + 1) % num_q]["url_id"]

        refs = refs | _find_md_links(data["questions"][i]["explain"])

    data["refs"] = refs

    return data


def _compile_once(no_wip):
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(config.SRC_ROOT),
        autoescape=jinja2.select_autoescape,
    )
    ctx = _load_data(no_wip)

    os.makedirs(config.BUILD_ROOT, exist_ok=True)

    _logger.info("Generating index.html file")
    template = env.get_template(config.INDEX_PAGE)
    _render_to_file("index.html", template, ctx)

    _logger.info("Generating browse.html file")
    template = env.get_template(config.BROWSE_PAGE)
    _render_to_file("browse.html", template, ctx)

    _logger.info("Generating challenge.html file")
    template = env.get_template(config.CHALLENGE_PAGE)
    _render_to_file("challenge.html", template, ctx)

    _logger.info("Generating faq.html file")
    template = env.get_template(config.FAQ_PAGE)
    _render_to_file("faq.html", template, ctx)

    _logger.info("Generating references.html file")
    template = env.get_template(config.REFERENCES_PAGE)
    _render_to_file("references.html", template, ctx)

    _logger.info("Generating single question .html files")
    template = env.get_template(config.SINGLE_Q_PAGE)
    base_path = os.path.join(config.BUILD_ROOT, "q")
    os.makedirs(base_path, exist_ok=True)
    for q in ctx["questions"]:
        q["explain_md"] = markdown.markdown(q["explain"])
        _render_to_file(
            os.path.join("q", f"{q['url_id']}.html"),
            template,
            {
                "q": q,
                **ctx,
            },
        )

    _logger.info("Generating single machine .html files")
    template = env.get_template(config.SINGLE_M_PAGE)
    os.makedirs(os.path.join(config.BUILD_ROOT, "m"), exist_ok=True)
    for m in ctx["machines"]:
        _render_to_file(
            os.path.join("m", f"{m['url_id']}.html"),
            template,
            {
                "m": m,
                **ctx,
            },
        )

    _logger.info("Copy static files")
    os.makedirs(os.path.join(config.BUILD_ROOT, "static"), exist_ok=True)
    shutil.copytree(
        os.path.join(config.SRC_ROOT, config.STATIC_DIR),
        os.path.join(config.BUILD_ROOT, "static"),
        dirs_exist_ok=True,
    )


@click.command()
@click.option("--watch", is_flag=True)
@click.option("--no_wip", is_flag=True)
def main(watch, no_wip):
    _compile_once(no_wip)

    if not watch:
        _logger.info("Run below command to run a test server")
        _logger.info(
            f"cd {os.path.abspath(config.BUILD_ROOT)} && python3 -m http.server"
        )
        _logger.info("Done.")
    else:
        observer = Observer()
        _logger.info(f"Watching directory {config.BUILD_ROOT}")

        class EventHandler(FileSystemEventHandler):
            def on_modified(self, event):
                _compile_once(no_wip)

        event_handler = EventHandler()
        observer.schedule(event_handler, config.SRC_ROOT, recursive=True)
        observer.start()

        try:
            while True:
                time.sleep(1)
        finally:
            observer.stop()
            observer.join()


if __name__ == "__main__":
    logging.basicConfig(
        encoding="utf-8",
        level=logging.INFO,
        format="%(asctime)s %(levelname)s [%(module)s:%(lineno)d] %(message)s",
        datefmt="%Y%m%d %H:%M:%S",
    )
    main()
