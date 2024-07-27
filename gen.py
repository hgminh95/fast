import logging
import os
import time
import shutil

import jinja2
import click
import yaml
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

import config


_logger = logging.getLogger(__name__)


def _render_to_file(filepath, template, ctx):
    fp = os.path.join(config.BUILD_ROOT, filepath)
    with open(fp, "wt") as f:
        f.write(template.render(**ctx))


def _load_data():
    fp = os.path.join(config.SRC_ROOT, config.DATA_FILE)

    with open(fp) as stream:
        data = yaml.safe_load(stream)

    for q in data["questions"]:
        q["url_id"] = q["title"].strip().replace(" ", "_").lower()

    return data


def _compile_once():
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(config.SRC_ROOT),
        autoescape=jinja2.select_autoescape,
    )
    ctx = _load_data()

    os.makedirs(config.BUILD_ROOT, exist_ok=True)

    _logger.info("Generating index.html file")
    template = env.get_template(config.INDEX_PAGE)
    _render_to_file("index.html", template, ctx)

    _logger.info("Generating browse.html file")
    template = env.get_template(config.BROWSE_PAGE)
    _render_to_file("browse.html", template, ctx)

    _logger.info("Generating challenge.html file")
    template = env.get_template(config.BROWSE_PAGE)
    _render_to_file("challenge.html", template, ctx)

    _logger.info("Generating random.html file")
    template = env.get_template(config.BROWSE_PAGE)
    _render_to_file("random.html", template, ctx)

    _logger.info("Generating single question .html files")
    template = env.get_template(config.SINGLE_Q_PAGE)
    base_path = os.path.join(config.BUILD_ROOT, "q")
    os.makedirs(base_path, exist_ok=True)
    for q in ctx["questions"]:
        _render_to_file(
            os.path.join("q", f"{q['url_id']}.html"),
            template,
            {
                "q": q,
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
def main(watch):
    _compile_once()

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
                _compile_once()

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